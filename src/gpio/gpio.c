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
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "gpio.h"

static int
maa_gpio_get_valfp(maa_gpio_context *dev)
{
    char bu[64];
    sprintf(bu, "/sys/class/gpio/gpio%d/value", dev->pin);

    if ((dev->value_fp = fopen(bu, "r+b")) == NULL) {
        return 1;
    }
    return 0;
}

maa_gpio_context*
maa_gpio_init(int pin)
{
    //TODO
    return maa_gpio_init_raw(pin);
}

maa_gpio_context*
maa_gpio_init_raw(int pin)
{
    FILE *export_f;
    maa_gpio_context* dev = (maa_gpio_context*) malloc(sizeof(maa_gpio_context));

    if ((export_f = fopen("/sys/class/gpio/export", "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return NULL;
    } else {
        fprintf(export_f, "%d", pin);
        fclose(export_f);
    }
    dev->pin = pin;
    return dev;
}

maa_result_t
maa_gpio_mode(maa_gpio_context *dev, gpio_mode_t mode)
{
    if (dev->value_fp != NULL) {
         dev->value_fp = NULL;
    }
    char filepath[64];
    snprintf(filepath, 64, "/sys/class/gpio/gpio%d/drive", dev->pin);

    FILE *drive;
    if ((drive = fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open drive for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }
    switch(mode) {
        case MAA_GPIO_STRONG:
            fprintf(drive, "strong");
            break;
        case MAA_GPIO_PULLUP:
            fprintf(drive, "pullup");
            break;
        case MAA_GPIO_PULLDOWN:
            fprintf(drive, "pulldown");
            break;
        case MAA_GPIO_HIZ:
            fprintf(drive, "hiz");
            break;
        default:
            fclose(drive);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
            break;
    }
    fclose(drive);
    dev->value_fp = NULL;
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_dir(maa_gpio_context *dev, gpio_dir_t dir)
{
    if (dev->value_fp != NULL) {
         dev->value_fp = NULL;
    }
    char filepath[64];
    snprintf(filepath, 64, "/sys/class/gpio/gpio%d/direction", dev->pin);

    FILE *direction;
    if ((direction = fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open direction for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }
    switch(dir) {
        case MAA_GPIO_OUT:
            fprintf(direction, "out");
            break;
        case MAA_GPIO_IN:
            fprintf(direction, "in");
            break;
        default:
            fclose(direction);
            return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
            break;
    }
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
    char buffer[2];
    fread(buffer, 2, 1, dev->value_fp);
    fseek(dev->value_fp, SEEK_SET, 0);
    return atoi(buffer);
}

maa_result_t
maa_gpio_write(maa_gpio_context *dev, int value)
{
    if (dev->value_fp == NULL) {
        maa_gpio_get_valfp(dev);
    }
    fseek(dev->value_fp, SEEK_SET, 0);
    fprintf(dev->value_fp, "%d", value);
    fseek(dev->value_fp, SEEK_SET, 0);
    if (ferror(dev->value_fp) != 0)
        return MAA_ERROR_INVALID_RESOURCE;
    return MAA_SUCCESS;
}

maa_result_t
maa_gpio_close(maa_gpio_context *dev)
{
    FILE *unexport_f;

    if ((unexport_f = fopen("/sys/class/gpio/unexport", "w")) == NULL) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return MAA_ERROR_INVALID_RESOURCE;
    }
    fprintf(unexport_f, "%d", dev->pin);
    fclose(unexport_f);
    if (ferror(dev->value_fp) != 0)
        return MAA_ERROR_INVALID_RESOURCE;
    free(dev);
    return MAA_SUCCESS;
}
