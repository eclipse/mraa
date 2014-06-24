/*
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

/**
 * @file
 * @brief Inter-Integrated Circuit
 *
 * This file defines the i2c/Iic interface for libmraa. A context represents a
 * bus and that bus may contain multiple addresses or i2c slaves. It is
 * considered best practice to make sure the address is correct before doing
 * any calls on i2c, in case another application or even thread changed the
 * addres on that bus. Multiple instances of the same bus can exist.
 *
 * @snippet i2c_HMC5883L.c Interesting
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>

#include "common.h"
#include "gpio.h"

/**
 * Opaque pointer definition to the internal struct _i2c
 */
typedef struct _i2c* mraa_i2c_context;

/**
 * Initialise i2c context, using board defintions
 *
 * @param bus i2c bus to use
 * @return i2c context or NULL
 */
mraa_i2c_context mraa_i2c_init(int bus);

/**
 * Initialise i2c context, passing in spi bus to use.
 *
 * @param bus The i2c bus to use i.e. /dev/i2c-2 would be "2"
 * @return i2c context or NULL
 */
mraa_i2c_context mraa_i2c_init_raw(unsigned int bus);

/**
 * Sets the frequency of the i2c context
 *
 * @param dev The i2c context
 * @param hz The bus frequency in hertz
 * @return Result of operation
 */
mraa_result_t mraa_i2c_frequency(mraa_i2c_context dev, int hz);

/**
 * Read from an i2c context
 *
 * @param dev The i2c context
 * @param data pointer to the byte array to read data in to
 * @param length max number of bytes to read
 * @return length of the read in bytes or 0
 */
int mraa_i2c_read(mraa_i2c_context dev, uint8_t *data, int length);

/**
 * Read a single byte from the i2c context
 *
 * @param dev The i2c context
 * @return The result of the read or -1 if failed
 */
uint8_t mraa_i2c_read_byte(mraa_i2c_context dev);

/**
 * Write to an i2c context
 *
 * @param dev The i2c context
 * @param data pointer to the byte array to be written
 * @param length the number of bytes to transmit
 * @return Result of operation
 */
mraa_result_t mraa_i2c_write(mraa_i2c_context dev, const uint8_t *data, int length);

/**
 * Write a single byte to an i2c context
 *
 * @param dev The i2c context
 * @param data The byte to write
 * @return Result of operation
 */
mraa_result_t mraa_i2c_write_byte(mraa_i2c_context dev, const uint8_t data);

/**
 * Sets the i2c context address.
 *
 * @param dev The i2c context
 * @param address The address to set for the slave (ignoring the least
 *   signifcant bit). If set to 0, the slave will only respond to the
 *   general call address.
 * @return Result of operation
 */
mraa_result_t mraa_i2c_address(mraa_i2c_context dev, int address);

/**
 * De-inits an mraa_i2c_context device
 *
 * @param dev The i2c context
 * @return Result of operation
 */
mraa_result_t mraa_i2c_stop(mraa_i2c_context dev);

#ifdef __cplusplus
}
#endif
