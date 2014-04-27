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

#include "pwm.h"

static int
maa_pwm_setup_duty_fp(pwm_t* dev)
{
    char bu[64];
    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", dev->chipid, dev->pin);

    if ((dev->duty_fp = fopen(bu, "r+b")) == NULL) {
        return 1;
    }
    return 0;
}

static void
maa_pwm_write_period(pwm_t* dev, int period)
{
    FILE *period_f;
    char bu[64];
    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/period", dev->chipid, dev->pin);

    if ((period_f = fopen(bu, "r+b")) == NULL) {
        fprintf(stderr, "Failed to open period for writing!\n");
    }
    fprintf(period_f, "%d", period);
    fclose(period_f);
}

static void
maa_pwm_write_duty(pwm_t* dev, int duty)
{
    if (dev->duty_fp == NULL) {
        maa_pwm_setup_duty_fp(dev);
    }
    fprintf(dev->duty_fp, "%d", duty);
    rewind(dev->duty_fp);
    fflush(dev->duty_fp);
}

static int
maa_pwm_get_period(pwm_t* dev)
{
    FILE *period_f;
    char bu[64];
    char output[16];

    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/period", dev->chipid, dev->pin);
    if ((period_f = fopen(bu, "rb")) == NULL) {
        fprintf(stderr, "Failed to open period for reading!\n");
        return 0;
    }
    fgets(output, 16, period_f);
    fclose(period_f);
    return atoi(output);
}

static int
maa_pwm_get_duty(pwm_t* dev)
{
    if (dev->duty_fp == NULL) {
        maa_pwm_setup_duty_fp(dev);
    }
    char output[16];
    fgets(output, 16, dev->duty_fp);
    fseek(dev->duty_fp, SEEK_SET, 0);
    return atoi(output);
}

maa_result_t
maa_pwm_init(pwm_t* dev, int chipin, int pin)
{
    dev = malloc(sizeof *dev);
    if (!dev)
        return MAA_ERROR_NO_RESOURCES;
    dev->chipid = chipin;
    dev->pin = pin;

    FILE *export_f;
    char buffer[64];
    snprintf(buffer, 64, "/sys/class/pwm/pwmchip%d/export", dev->chipid);

    if ((export_f = fopen(buffer, "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
	return MAA_ERROR_INVALID_HANDLE;
    } else {
        fprintf(export_f, "%d", dev->pin);
        fclose(export_f);
        maa_pwm_setup_duty_fp(dev);
    }

    return MAA_SUCCESS;
}

void
maa_pwm_write(pwm_t* dev, float percentage)
{
    maa_pwm_write_duty(dev, percentage * maa_pwm_get_period(dev));
}

float
maa_pwm_read(pwm_t* dev)
{
    float output = maa_pwm_get_duty(dev) / (float) maa_pwm_get_period(dev);
    return output;
}

void
maa_pwm_period(pwm_t* dev, float seconds)
{
    maa_pwm_period_ms(dev, seconds*1000);
}

void
maa_pwm_period_ms(pwm_t* dev, int ms)
{
    maa_pwm_period_us(dev, ms*1000);
}

void
maa_pwm_period_us(pwm_t* dev, int us)
{
    maa_pwm_write_period(dev, us*1000);
}

void
maa_pwm_pulsewidth(pwm_t* dev, float seconds)
{
    maa_pwm_pulsewidth_ms(dev, seconds*1000);
}

void
maa_pwm_pulsewidth_ms(pwm_t* dev, int ms)
{
    maa_pwm_pulsewidth_us(dev, ms*1000);
}

void
maa_pwm_pulsewidth_us(pwm_t* dev, int us)
{
    maa_pwm_write_duty(dev, us*1000);
}

void
maa_pwm_enable(pwm_t* dev, int enable)
{
    int status;
    if (enable != 0) {
        status = 1;
    } else {
        status = enable;
    }
    FILE *enable_f;
    char bu[64];
    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/enable", dev->chipid, dev->pin);

    if ((enable_f = fopen(bu, "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
    } else {
        fprintf(enable_f, "%d", status);
        fclose(enable_f);
    }
    //Do Something
}

void
maa_pwm_close(pwm_t* dev)
{
    maa_pwm_enable(dev, 0);
    FILE *unexport_f;
    char buffer[64];
    snprintf(buffer, 64, "/sys/class/pwm/pwmchip%d/unexport", dev->chipid);

    if ((unexport_f = fopen(buffer, "w")) == NULL) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
    } else {
        fprintf(unexport_f, "%d", dev->pin);
        fclose(unexport_f);
    }
    free(dev);
}
