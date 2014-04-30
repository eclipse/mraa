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
 * This file defines the i2c interface for libmaa
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "maa.h"
#include "gpio.h"

/**
 * A structure representing an i2c device /dev/i2c-*
 */
typedef struct {
    /*@{*/
    int hz; /**< frequency of communication */
    int fh; /**< the file handle to the /dev/i2c-* device */
    int addr; /**< the address of the i2c slave */
    maa_gpio_context gpio;
    /*@}*/
} maa_i2c_context;

maa_i2c_context* maa_i2c_init();

/** Set the frequency of the I2C interface
 *
 *  @param dev the i2c context
 *  @param hz The bus frequency in hertz
 *
 *  @return maa_result_t the maa result.
 */
maa_result_t maa_i2c_frequency(maa_i2c_context* dev, int hz);

/** Checks to see if this I2C Slave has been addressed.
 *
 *  @param dev the i2c context
 *
 *  @return maa_result_t the maa result.
 */
maa_result_t maa_i2c_receive(maa_i2c_context* dev);

/** Read from an I2C master.
 *
 *  @param dev the i2c context
 *  @param data pointer to the byte array to read data in to
 *  @param length maximum number of bytes to read
 *
 *  @return maa_result_t the maa result.
 */
maa_result_t maa_i2c_read(maa_i2c_context* dev, char *data, int length);

/** Read a single byte from an I2C master.
 *
 *  @param dev the i2c context
 *
 *  @return byte the result of the read or -1 if failed.
 */
int maa_i2c_read_byte(maa_i2c_context* dev);

/** Write to an I2C master
 *
 *  @param dev the i2c context
 *  @param data pointer to the byte array to be transmitted
 *  @param length the number of bytes to transmite
 *
 *  @return maa_result_t the maa result.
 */
maa_result_t maa_i2c_write(maa_i2c_context* dev, const char *data, int length);

/** Write a single byte to an I2C master.
 *
 *  @param dev the i2c context
 *  @data the byte to write
 *
 *  @return maa_result_t the maa result.
 */
maa_result_t maa_i2c_write_byte(maa_i2c_context* dev, int data);

/** Sets the I2C slave address.
 *
 *  @param dev the i2c context
 *  @param address The address to set for the slave (ignoring the least
 *  signifcant bit). If set to 0, the slave will only respond to the
 *  general call address.
 *
 *  @return maa_result_t the maa result.
 */
maa_result_t maa_i2c_address(maa_i2c_context* dev, int address);

/** De-inits an maa_i2c_context device
 *
 *  @param dev the i2c context
 *
 *  @return maa_result_t the maa result.
 */
maa_result_t maa_i2c_stop(maa_i2c_context* dev);

#ifdef __cplusplus
}
#endif
