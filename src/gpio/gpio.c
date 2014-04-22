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

#ifdef __cplusplus
extern "C" {
#endif

int
gpio_get_valfp(gpio_t *gpio);

void
gpio_init(gpio_t *gpio, int pin) {
    FILE *export_f;

    if((export_f = fopen("/sys/class/gpio/export", "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
    } else {
        fprintf(export_f, "%d", pin);
        fclose(export_f);
    }
    gpio->pin = pin;
}

int
gpio_set(int pin) {
    //Stuff
    return 0;
}

void
gpio_mode(gpio_t *gpio, gpio_mode_t mode) {
    //gpio->pin
}

void
gpio_dir(gpio_t *gpio, gpio_dir_t dir) {
    if(gpio->value_fp != NULL) {
         gpio->value_fp = NULL;
    }
    char filepath[64];
    snprintf(filepath, 64, "/sys/class/gpio/gpio%d/direction", gpio->pin);

    FILE *direction;
    if((direction = fopen(filepath, "w")) == NULL) {
        fprintf(stderr, "Failed to open direction for writing!\n");
    } else {
        fprintf(direction, dir);
        fclose(direction);
        gpio->value_fp = NULL;
    }
}

int
gpio_read(gpio_t *gpio) {
    if(gpio->value_fp == NULL) {
        gpio_get_valfp(gpio);
    }
    fseek(gpio->value_fp, SEEK_SET, 0);
    char buffer[2];
    fread(buffer, 2, 1, gpio->value_fp);
    fseek(gpio->value_fp, SEEK_SET, 0);
    return atoi(buffer);
}

void
gpio_write(gpio_t *gpio, int value) {
    if(gpio->value_fp == NULL) {
        gpio_get_valfp(gpio);
    }
    fseek(gpio->value_fp, SEEK_SET, 0);
    fprintf(gpio->value_fp, "%d", value);
    fseek(gpio->value_fp, SEEK_SET, 0);

}

void
gpio_close(gpio_t *gpio) {
    FILE *unexport_f;

    if((unexport_f = fopen("/sys/class/gpio/unexport", "w")) == NULL) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
    } else {
        fprintf(unexport_f, "%d", gpio->pin);
        fclose(unexport_f);
    }
}

int
gpio_get_valfp(gpio_t *gpio) {
    char bu[64];
    sprintf(bu, "/sys/class/gpio/gpio%d/value", gpio->pin);

    if((gpio->value_fp = fopen(bu, "r+b")) == NULL) {
        return 1;
    } else {
        return 0;
    }
    return 1;
}

#ifdef __cplusplus
}
#endif
