/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <sys/stat.h>

#include "pwm.h"
#include "maa_internal.h"

#define MAX_SIZE 64
#define SYSFS_PWM "/sys/class/pwm"

/**
 * A structure representing a PWM pin
 */
struct _pwm {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    int chipid; /**< the chip id, which the pwm resides */
    int duty_fp; /**< File pointer to duty file */
    maa_boolean_t owner; /**< Owner of pwm context*/
    /*@}*/
};

static int
maa_pwm_setup_duty_fp(maa_pwm_context dev)
{
    char bu[MAX_SIZE];
    snprintf(bu,MAX_SIZE, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", dev->chipid, dev->pin);

    dev->duty_fp = open(bu, O_RDWR);
    if (dev->duty_fp == -1) {
        return 1;
    }
    return 0;
}

static maa_result_t
maa_pwm_write_period(maa_pwm_context dev, int period)
{
    char bu[MAX_SIZE];
    snprintf(bu,MAX_SIZE ,"/sys/class/pwm/pwmchip%d/pwm%d/period", dev->chipid, dev->pin);

    int period_f = open(bu, O_RDWR);
    if (period_f == -1) {
        fprintf(stderr, "Failed to open period for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }
    char out[MAX_SIZE];
    int length = snprintf(out, MAX_SIZE, "%d", period);
    if (write(period_f, out, length*sizeof(char)) == -1) {
        close(period_f);
        return MAA_ERROR_INVALID_RESOURCE;
    }

    close(period_f);
    return MAA_SUCCESS;
}

static maa_result_t
maa_pwm_write_duty(maa_pwm_context dev, int duty)
{
    if (dev->duty_fp == -1) {
        maa_pwm_setup_duty_fp(dev);
    }
    char bu[64];
    int length = sprintf(bu, "%d", duty);
    if (write(dev->duty_fp, bu, length * sizeof(char)) == -1)
        return MAA_ERROR_INVALID_RESOURCE;
    return MAA_SUCCESS;
}

static int
maa_pwm_get_period(maa_pwm_context dev)
{
    char bu[MAX_SIZE];
    char output[MAX_SIZE];
    snprintf(bu,MAX_SIZE, "/sys/class/pwm/pwmchip%d/pwm%d/period", dev->chipid, dev->pin);

    int period_f = open(bu, O_RDWR);
    if (period_f == -1) {
        fprintf(stderr, "Failed to open period for reading!\n");
        return 0;
    }
    off_t size = lseek(period_f, 0, SEEK_END);
    lseek(period_f, 0, SEEK_SET);

    read(period_f, output, size + 1);
    close(period_f);
    int ret = strtol(output, NULL, 10);

    return ret;
}

static int
maa_pwm_get_duty(maa_pwm_context dev)
{
    if (dev->duty_fp == -1) {
        maa_pwm_setup_duty_fp(dev);
    } else {
        lseek(dev->duty_fp, 0, SEEK_SET);
    }
    off_t size = lseek(dev->duty_fp, 0, SEEK_END);
    lseek(dev->duty_fp, 0, SEEK_SET);
    char output[MAX_SIZE];
    read(dev->duty_fp, output, size+1);

    int ret = strtol(output, NULL, 10);
    return ret;
}

maa_pwm_context
maa_pwm_init(int pin) {
    maa_pin_t* pinm = maa_setup_pwm(pin);
    if (pinm == NULL)
        return NULL;
    int chip = pinm->parent_id;
    int pinn = pinm->pinmap;
    free(pinm);
    return maa_pwm_init_raw(chip,pinn);
}

maa_pwm_context
maa_pwm_init_raw(int chipin, int pin)
{
    maa_pwm_context dev = (maa_pwm_context) malloc(sizeof(struct _pwm));
    if (dev == NULL)
        return NULL;
    dev->duty_fp == -1;
    dev->chipid = chipin;
    dev->pin = pin;

    char directory[MAX_SIZE];
    snprintf(directory, MAX_SIZE, SYSFS_PWM "/pwmchip%d/pwm%d", dev->chipid, dev->pin);
    struct stat dir;
    if (stat(directory, &dir) == 0 && S_ISDIR(dir.st_mode)) {
        fprintf(stderr, "PWM Pin already exporting, continuing.\n");
        dev->owner = 0; // Not Owner
    } else {
        char buffer[MAX_SIZE];
        snprintf(buffer, MAX_SIZE, "/sys/class/pwm/pwmchip%d/export", dev->chipid);
        int export_f = open(buffer, O_WRONLY);
        if (export_f == -1) {
            fprintf(stderr, "Failed to open export for writing!\n");
            free(dev);
            return NULL;
        }

        char out[MAX_SIZE];
        int size = snprintf(out, MAX_SIZE, "%d", dev->pin);
        if (write(export_f, out, size*sizeof(char)) == -1) {
            fprintf(stderr, "Failed to write to export! Potentially already enabled\n");
            close(export_f);
            return NULL;
        }
        dev->owner = 1;
        close(export_f);
    }
    maa_pwm_setup_duty_fp(dev);
    return dev;
}

maa_result_t
maa_pwm_write(maa_pwm_context dev, float percentage)
{
    return maa_pwm_write_duty(dev, percentage * maa_pwm_get_period(dev));
}

float
maa_pwm_read(maa_pwm_context dev)
{
    float output = maa_pwm_get_duty(dev) / (float) maa_pwm_get_period(dev);
    return output;
}

maa_result_t
maa_pwm_period(maa_pwm_context dev, float seconds)
{
    return maa_pwm_period_ms(dev, seconds*1000);
}

maa_result_t
maa_pwm_period_ms(maa_pwm_context dev, int ms)
{
    return maa_pwm_period_us(dev, ms*1000);
}

maa_result_t
maa_pwm_period_us(maa_pwm_context dev, int us)
{
    return maa_pwm_write_period(dev, us*1000);
}

maa_result_t
maa_pwm_pulsewidth(maa_pwm_context dev, float seconds)
{
    return maa_pwm_pulsewidth_ms(dev, seconds*1000);
}

maa_result_t
maa_pwm_pulsewidth_ms(maa_pwm_context dev, int ms)
{
    return maa_pwm_pulsewidth_us(dev, ms*1000);
}

maa_result_t
maa_pwm_pulsewidth_us(maa_pwm_context dev, int us)
{
    return maa_pwm_write_duty(dev, us*1000);
}

maa_result_t
maa_pwm_enable(maa_pwm_context dev, int enable)
{
    int status;
    if (enable != 0) {
        status = 1;
    } else {
        status = enable;
    }
    char bu[MAX_SIZE];
    snprintf(bu,MAX_SIZE, "/sys/class/pwm/pwmchip%d/pwm%d/enable", dev->chipid, dev->pin);

    int enable_f = open(bu, O_RDWR);

    if (enable_f == -1) {
        fprintf(stderr, "Failed to open enable for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }
    char out[2];
    int size = snprintf(out, sizeof(out), "%d", enable);
    if (write(enable_f, out, size * sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to enable!\n");
        close(enable_f);
        return MAA_ERROR_INVALID_RESOURCE;
    }
    close(enable_f);
    return MAA_SUCCESS;
}

maa_result_t
maa_pwm_unexport(maa_pwm_context dev)
{
    if (dev->owner) {
        return maa_pwm_unexport_force(dev);
    }
    return MAA_ERROR_INVALID_RESOURCE;
}

maa_result_t
maa_pwm_unexport_force(maa_pwm_context dev)
{
    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, "/sys/class/pwm/pwmchip%d/unexport", dev->chipid);

    int unexport_f = open(filepath, O_WRONLY);
    if (unexport_f == -1) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char out[MAX_SIZE];
    int size = snprintf(out, MAX_SIZE, "%d", dev->pin);
    if (write(unexport_f, out, size*sizeof(char)) == -1) {
        fprintf(stderr, "Failed to write to unexport!\n");
        close(unexport_f);
        return MAA_ERROR_INVALID_RESOURCE;
    }

    close(unexport_f);
    return MAA_SUCCESS;
}

maa_result_t
maa_pwm_close(maa_pwm_context dev)
{
    maa_pwm_unexport(dev);
    free(dev);
    return MAA_SUCCESS;
}

maa_result_t
maa_pwm_owner(maa_pwm_context dev, maa_boolean_t owner_new)
{
    if (dev == NULL)
        return MAA_ERROR_INVALID_RESOURCE;
    dev->owner = owner_new;
    return MAA_SUCCESS;
}
