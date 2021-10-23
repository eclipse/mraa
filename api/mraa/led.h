/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2017 Linaro Ltd.
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
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

/**
 * @file
 * @brief LED module
 *
 * LED is the Light Emitting Diode interface to libmraa. It is used to
 * access the on board LED's via sysfs.
 *
 * @snippet led.c Interesting
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdio.h>

/**
 * Opaque pointer definition to the internal struct _led
 */
typedef struct _led* mraa_led_context;

/**
 * Initialise led_context, based on led index.
 *
 *  @param led ID of the LED
 *  @returns LED context or NULL
 */
mraa_led_context mraa_led_init(int led);

/**
 * Initialise led_context, based on led function name.
 * The structure of LED entry in sysfs is "devicename:colour:function"
 * This api expects only one unique LED identifier which would be
 * "function" name most often. For instance, `mraa_led_init_raw("user4");`
 *
 *  @param led_dev Name of the LED device
 *  @returns LED context or NULL
 */
mraa_led_context mraa_led_init_raw(const char* led_dev);

/**
 * Set LED brightness
 *
 *  @param dev LED context
 *  @param value Integer value to write
 *  @returns Result of operation
 */
mraa_result_t mraa_led_set_brightness(mraa_led_context dev, int value);

/**
 * Read LED brightness
 *
 *  @param dev LED context
 *  @returns Brightness value
 */
int mraa_led_read_brightness(mraa_led_context dev);

/**
 * Read LED maximum brightness
 *
 *  @param dev LED context
 *  @returns Maximum brightness value
 */
int mraa_led_read_max_brightness(mraa_led_context dev);

/**
 * Set LED trigger
 *
 *  @param dev LED context
 *  @param trigger Type of trigger to set
 *  @returns Result of operation
 */
mraa_result_t mraa_led_set_trigger(mraa_led_context dev, const char* trigger);

/**
 * Clear active LED trigger
 *
 *  @param dev LED context
 *  @returns Result of operation
 */
mraa_result_t mraa_led_clear_trigger(mraa_led_context dev);

/**
 * Close LED file descriptors and free the context memory
 *
 *  @param dev LED context
 *  @returns Result of operation
 */
mraa_result_t mraa_led_close(mraa_led_context dev);

#ifdef __cplusplus
}
#endif
