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

#pragma once

#include "common.h"
#include "mraa.h"

// general status failures for internal functions
#define MRAA_PLATFORM_NO_INIT -3
#define MRAA_IO_SETUP_FAILURE -2
#define MRAA_NO_SUCH_IO -1

/**
 * A structure representing a gpio pin.
 */
struct _gpio {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    int phy_pin; /**< pin passed to clean init. -1 none and raw*/
    int value_fp; /**< the file pointer to the value of the gpio */
    void (* isr)(void *); /**< the interupt service request */
    void *isr_args; /**< args return when interupt service request triggered */
    pthread_t thread_id; /**< the isr handler thread id */
    int isr_value_fp; /**< the isr file pointer on the value */
    mraa_boolean_t owner; /**< If this context originally exported the pin */
    mraa_result_t (*mmap_write) (mraa_gpio_context dev, int value);
    int (*mmap_read) (mraa_gpio_context dev);
    /*@}*/
};

/**
 * A structure representing a I2C bus
 */
struct _i2c {
    /*@{*/
    int fh; /**< the file handle to the /dev/i2c-* device */
    int addr; /**< the address of the i2c slave */
    /*@}*/
};

/**
 * A structure representing a PWM pin
 */
struct _pwm {
    /*@{*/
    int pin; /**< the pin number, as known to the os. */
    int chipid; /**< the chip id, which the pwm resides */
    int duty_fp; /**< File pointer to duty file */
    mraa_boolean_t owner; /**< Owner of pwm context*/
    /*@}*/
};

/**
 * A structure representing a Analog Input Channel
 */
struct _aio {
    unsigned int channel; /**< the channel as on board and ADC module */
    int adc_in_fp; /**< File Pointer to raw sysfs */
    int value_bit; /**< 10 bits by default. Can be increased if board */
};

/**
 * A structure representing a UART device
 */
struct _uart {
    /*@{*/
    int index; /**< the uart index, as known to the os. */
    /*@}*/
};
