/*
 * Copyright (C) Intel Corporation.
 *
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 *
 * Copyright © 2014 Intel Corporation
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
#include "gpio.h"

using namespace maa;

void
gpio_init(gpio_t *gpio, int pin) {
    malloc(sizeof(gpio_t));
    gpio->pin = pin;
}
int
gpio_set(gpio_t *gpio) {
    //Stuff
    return 0;
}

void
gpio_mode(gpio_t *gpio, gpio_mode_t mode) {
    //gpio->pin
}
void
gpio_dir(gpio_t *gpio, gpio_dir_t dir) {

}
int
gpio_read(gpio_t *gpio) {
    return 0;
}
void
gpio_write(gpio_t *gpio, int value){

}

