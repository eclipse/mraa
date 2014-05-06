/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "gpio.h"

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64

static int
maa_gpio_get_valfp(maa_gpio_context *dev)
{
    char bu[MAX_SIZE];
    sprintf(bu, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);

    if ((dev->value_fp = fopen(bu, "r+b")) == NULL) {
        return 1;
    }

    return 0;
}

maa_gpio_context*
maa_gpio_init(int pin)
{
    int pinm = maa_check_gpio(pin);
    if (pinm < 0)
        return NULL;

    return maa_gpio_init_raw(pinm);
}

maa_gpio_context*
maa_gpio_init_raw(int pin)
{
    if (pin < 0)
        return NULL;

    FILE *export_f;
    char bu[MAX_SIZE];
    int length;

    maa_gpio_context* dev = (maa_gpio_context*) malloc(sizeof(maa_gpio_context));
    dev->pin = pin;

    if ((export_f = fopen(SYSFS_CLASS_GPIO "/export", "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return NULL;
    }
    length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    fwrite(bu, sizeof(char), length, export_f);

    fclose(export_f);
    return dev;
}

maa_result_t
maa_gpio_mode(maa_gpio_context *dev, gpio_mode_t mode)
{
    if (dev->value_fp != NULL) {
         dev->value_fp = NULL;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/drive", dev->pin);

    FILE *drive;
    if ((drive = fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open drive for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(mode) {
        case MAA_GPIO_STRONG:
            length = snprintf(bu, sizeof(bu), "strong");
            break;
        case MAA_GPIO_PULLUP:
            length = snprintf(bu, sizeof(bu), "pullup");
            break;
        case MAA_GPIO_PULLDOWN:
            length = snprintf(bu, sizeof(bu), "pulldown");
            break;
        case MAA_GPIO_HIZ:
            length = snprintf(bu, sizeof(bu), "hiz");
            break;
        default:
            fclose(drive);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    fwrite(bu, sizeof(char), length, drive);

    fclose(drive);
    dev->value_fp = NULL;
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_dir(maa_gpio_context *dev, gpio_dir_t dir)
{
    if (dev == NULL)
        return MAA_ERROR_INVALID_HANDLE;
    if (dev->value_fp != NULL) {
         dev->value_fp = NULL;
    }
    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", dev->pin);

    FILE *direction;
    if ((direction = fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open direction for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch(dir) {
        case MAA_GPIO_OUT:
            length = snprintf(bu, sizeof(bu), "out");
            break;
        case MAA_GPIO_IN:
            length = snprintf(bu, sizeof(bu), "in");
            break;
        default:
            fclose(direction);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }
    fwrite(bu, sizeof(char), length, direction);

    fclose(direction);
    dev->value_fp = NULL;
    return MAA_SUCCESS;
}

int
maa_gpio_read(maa_gpio_context *dev)
{
    if (dev->value_fp == NULL) {
        maa_gpio_get_valfp(dev);
    }
    fseek(dev->value_fp, SEEK_SET, 0);
    char bu[2];
    fread(bu, 2, 1, dev->value_fp);
    fseek(dev->value_fp, SEEK_SET, 0);
    int ret = strtol(bu, NULL, 10);

    return ret;
}

maa_result_t
maa_gpio_write(maa_gpio_context *dev, int value)
{
    if (dev->value_fp == NULL) {
        maa_gpio_get_valfp(dev);
    }
    if (fseek(dev->value_fp, SEEK_SET, 0) != 0) {
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", value);
    fwrite(bu, sizeof(char), length, dev->value_fp);

    if (fseek(dev->value_fp, SEEK_SET, 0) != 0) {
        return MAA_ERROR_INVALID_RESOURCE;
    }
    if (ferror(dev->value_fp) != 0)
        return MAA_ERROR_INVALID_RESOURCE;
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_unexport(maa_gpio_context *dev)
{
    FILE *unexport_f;

    if ((unexport_f = fopen(SYSFS_CLASS_GPIO "/unexport", "w")) == NULL) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    fwrite(bu, sizeof(char), length, unexport_f);
    fclose(unexport_f);

    if (ferror(dev->value_fp) != 0) {
        return MAA_ERROR_INVALID_RESOURCE;
    }
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_close(maa_gpio_context *dev)
{
    maa_gpio_unexport(dev);
    free(dev);
    return MAA_SUCCESS;
}
