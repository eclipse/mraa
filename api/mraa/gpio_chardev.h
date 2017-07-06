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
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include <stdio.h>

#define MAX_BUF_SIZE 64
#define CDEV_PREFIX "/dev/gpiochip"

/**
 * Opaque pointer definition to the internal struct _gpio
 */
typedef struct _gpio* mraa_gpio_char_context;

/**
 * Gpio Direction options
 */
typedef enum {
    MRAA_GPIO_OUTPUT = 0, /**< Output */
    MRAA_GPIO_INPUT = 1,  /**< Input */
} mraa_gpio_char_dir_t;

/**
 * Gpio State options
 */
typedef enum {
    MRAA_GPIO_STATE_ACTIVE_HIGH = 0, /**< Resistive High */
    MRAA_GPIO_STATE_ACTIVE_LOW = 1,  /**< Resistive Low */
} mraa_gpio_char_state_t;

/**
 * Gpio Output Mode options
 */
typedef enum {
    MRAA_GPIO_OUTPUT_OPEN_DRAIN = 0,  /**< Open Drain mode */
    MRAA_GPIO_OUTPUT_OPEN_SOURCE = 1, /**< Open Source mode */
} mraa_gpio_char_mode_t;

/**
 * Gpio Default value options
 */
typedef enum {
    MRAA_GPIO_DEFAULT_LOW = 0, /**< Default Output Low> */
    MRAA_GPIO_DEFAULT_HIGH = 1 /**< Default output High> */
} mraa_gpio_char_value_t;

/**
 * Initialise gpio_context, based on gpiochip and board number
 *
 *  @param chip Chip number in which gpio line belongs to
 *  @param pin Board pin number
 *  @returns gpio context or NULL
 */
mraa_gpio_char_context mraa_gpio_char_init(unsigned int chip, unsigned int pin);

/**
 * Set Gpio direction to input
 *
 * @param dev Gpio context
 * @param state active state of the Gpio (active-low/high)
 * @return Result of operation
 */
mraa_result_t mraa_gpio_char_set_input(mraa_gpio_char_context dev, mraa_gpio_char_state_t state);

/**
 * Set Gpio direction to output
 *
 * @param dev Gpio context
 * @param state active state of the Gpio (active-low/high)
 * @param mode Mode of the gpio (open-drain/source)
 * @param val Default value of the gpio (0/1)
 * @return Result of operation
 */
mraa_result_t mraa_gpio_char_set_output(mraa_gpio_char_context dev,
                                        mraa_gpio_char_state_t state,
                                        mraa_gpio_char_mode_t mode,
                                        mraa_gpio_char_value_t val);

/**
 * Read the Gpio value. This can be 0 or 1.
 *
 * @param dev The Gpio context
 * @return Gpio value
 */
int mraa_gpio_char_read(mraa_gpio_char_context dev);

/**
 * Write to the Gpio.
 *
 * @param dev The Gpio context
 * @param value Integer value to write
 * @return Result of operation
 */
mraa_result_t mraa_gpio_char_write(mraa_gpio_char_context dev, int val);

/**
 * Read Gpio direction
 *
 * @param dev The Gpio context
 * @param dir The address where to store the Gpio direction
 * @return Result of operation
 */
mraa_result_t mraa_gpio_char_read_dir(mraa_gpio_char_context dev, mraa_gpio_char_dir_t* dir);

/**
 * Read Gpio state
 *
 * @param dev The Gpio context
 * @param dir The address where to store the Gpio state
 * @return Result of operation
 */
mraa_result_t mraa_gpio_char_read_state(mraa_gpio_char_context dev, mraa_gpio_char_state_t* state);

/**
 * Close the Gpio context
 * - Will free the memory for the context and close the file descriptors
 *
 * @param dev The Gpio context
 * @return Result of operation
 */
mraa_result_t mraa_gpio_char_close(mraa_gpio_char_context dev);

/**
 * Get the board number of gpio context, invalid will return -1
 *
 * @param dev The Gpio context
 * @return Pin number
 */
int mraa_gpio_char_get_pin(mraa_gpio_char_context dev);

/**
 * Get the pin number of gpio context used internally, invalid will return -1
 *
 * @param dev The Gpio context
 * @return gpio number
 */
int mraa_gpio_char_get_pin_raw(mraa_gpio_char_context dev);

#ifdef __cplusplus
}
#endif
