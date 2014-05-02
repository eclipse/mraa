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

#pragma once

/** @file
 *
 * This file defines the gpio interface for libmaa
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "maa.h"
/**
 * A strucutre representing a gpio pin.
 */
typedef struct {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    FILE *value_fp; /**< the file pointer to the value of the gpio */
    /*@}*/
} maa_gpio_context;

/**
 * GPIO Output modes
 */
typedef enum {
    MAA_GPIO_STRONG     = 0, /**< Default. Strong high and low */
    MAA_GPIO_PULLUP     = 1, /**< Resistive High */
    MAA_GPIO_PULLDOWN   = 2, /**< Resistive Low */
    MAA_GPIO_HIZ        = 3  /**< High Z State */
} gpio_mode_t;

/**
 * GPIO Direction options.
 */
typedef enum {
    MAA_GPIO_OUT    = 0, /**< Output. A Mode can also be set */
    MAA_GPIO_IN     = 1  /**< Input. */
} gpio_dir_t;

/** Initialise gpio_context, based on board number
 *
 *  @param pin pin number read from the board, i.e IO3 is 3.
 *
 *  @returns maa_gpio_context based on the IO pin
 */
maa_gpio_context* maa_gpio_init(int pin);

/** Initialise gpio context without any mapping to a pin.
 * - For more expert users
 *
 * @param gpiopin gpio pin as listed in SYSFS
 *
 * @return gpio context
 */
maa_gpio_context* maa_gpio_init_raw(int gpiopin);

/** Set GPIO Output Mode,
 *
 * @param dev The GPIO context
 * @param mode The GPIO Output Mode.
 *
 * @return maa result type.
 */
maa_result_t maa_gpio_mode(maa_gpio_context *dev, gpio_mode_t mode);

/** Set GPIO direction
 *
 * @param dev The GPIO context.
 * @param dir The direction of the GPIO.
 *
 * @return maa result type.
 */
maa_result_t maa_gpio_dir(maa_gpio_context *dev, gpio_dir_t dir);

/** Close the GPIO context
 * - Will free the memory for the context and unexport the GPIO
 *
 * @param dev the GPIO context
 *
 * @return maa result type.
 */
maa_result_t maa_gpio_close(maa_gpio_context *dev);

/** Unexport the GPIO context (maa_gpio_close() will call this function)
 *
 * @param dev The GPIO context.
 *
 * @return maa result type.
 */
maa_result_t maa_gpio_unexport(maa_gpio_context *dev);

/** Read the GPIO value.
 *
 * @param dev The GPIO context.
 *
 * @return the integer value of the GPIO
 */
int maa_gpio_read(maa_gpio_context *dev);

/** Write to the GPIO Value.
 *
 * @param dev The GPIO context.
 * @param value Integer value to write.
 *
 * @return maa result type
 */
maa_result_t maa_gpio_write(maa_gpio_context *dev, int value);

#ifdef __cplusplus
}
#endif
