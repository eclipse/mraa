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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "maa.h"
#include "gpio.h"

typedef struct {
    int hz;
    int fh;
    int addr;
    gpio_t gpio;
} i2c_t;

maa_result_t maa_i2c_init(i2c_t* dev);

/** Set the frequency of the I2C interface
 *
 *  @param hz The bus frequency in hertz
 */
void maa_i2c_frequency(i2c_t* dev, int hz);

/** Checks to see if this I2C Slave has been addressed.
 *
 *  @returns
 *  A status indicating if the device has been addressed, and how
 *  - NoData            - the slave has not been addressed
 *  - ReadAddressed     - the master has requested a read from this slave
 *  - WriteAddressed    - the master is writing to this slave
 *  - WriteGeneral      - the master is writing to all slave
 */
int maa_i2c_receive(i2c_t* dev);

/** Read from an I2C master.
 *
 *  @param data pointer to the byte array to read data in to
 *  @param length maximum number of bytes to read
 *
 *  @returns
 *       0 on success,
 *   non-0 otherwise
 */
int maa_i2c_read(i2c_t* dev, char *data, int length);

/** Read a single byte from an I2C master.
 *
 *  @returns
 *    the byte read
 */
int maa_i2c_read_byte(i2c_t* dev);

/** Write to an I2C master.
 *
 *  @param data pointer to the byte array to be transmitted
 *  @param length the number of bytes to transmite
 *
 *  @returns
 *       0 on success,
 *   non-0 otherwise
 */
int maa_i2c_write(i2c_t* dev, const char *data, int length);

/** Write a single byte to an I2C master.
 *
 *  @data the byte to write
 *
 *  @returns
 *    '1' if an ACK was received,
 *    '0' otherwise
 */
int maa_i2c_write_byte(i2c_t* dev, int data);

/** Sets the I2C slave address.
 *
 *  @param address The address to set for the slave (ignoring the least
 *  signifcant bit). If set to 0, the slave will only respond to the
 *  general call address.
 */
void maa_i2c_address(i2c_t* dev, int address);

/** De-inits an i2c_t device
 */
void maa_i2c_stop(i2c_t* dev);
