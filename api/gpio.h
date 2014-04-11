/*
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

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpio_struct
{
    int pin;
    int pinMap;
    char path[64];
    FILE *value_fp;
} gpio_t;

typedef char gpio_mode_t[16];
typedef char gpio_dir_t[16];

void gpio_init(gpio_t *gpio, int pin);
int gpio_set(int pin);
void gpio_mode(gpio_t *gpio, gpio_mode_t mode);
void gpio_dir(gpio_t *gpio, gpio_dir_t dir);

void gpio_close(gpio_t *gpio);
int gpio_read(gpio_t *gpio);
void gpio_write(gpio_t *gpio, int value);

#ifdef __cplusplus
}
#endif
