/*
 * Originally from mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 * Copyright (c) 2014 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
    FILE *value_fp; /**< the file pointer to the value of the gpio /*
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
 *  @returns
 *       maa_gpio_context based on the IO pin
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
 * - Will free the memory for the context.
 *
 * @param dev the GPIO context
 *
 * @return maa result type.
 */
maa_result_t maa_gpio_close(maa_gpio_context *dev);

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
