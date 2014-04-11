/*
 * Copyright (C) Intel Corporation.
 *
 * Author: Brendan Le Foll
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

#include "i2cslave.h"

using namespace maa;

I2CSlave::I2CSlave(unsigned int sda, unsigned int scl)
{
    // Galileo only has one I2CSlave device which is always /dev/i2c-0
    // reliability is a fickle friend!
    if (i2c_handle = open("/dev/i2c-0", O_RDWR) < 1) {
        fprintf(stderr, "Failed to open requested i2c port");
    }
}

void
I2CSlave::frequency(int hz)
{
    _hz = hz;
}

int
I2CSlave::receive(void)
{
    return -1;
}

int
I2CSlave::read(char *data, int length)
{
    if (this->read(data, length) == length) {
        return length;
    }
    return -1;
}

int
I2CSlave::read(void)
{
    int byte;
    if (byte = i2c_smbus_read_byte(i2c_handle) < 0) {
        return -1;
    }
    return byte;
}

int
I2CSlave::write(const char *data, int length)
{
    if (i2c_smbus_write_i2c_block_data(i2c_handle, data[0], length, (uint8_t*) data) < 0) {
        fprintf(stderr, "Failed to write to I2CSlave slave\n");
	return -1;
    }
    return 0;
}

int
I2CSlave::write(int data)
{
    if (i2c_smbus_write_byte(i2c_handle, data) < 0) {
        fprintf(stderr, "Failed to write to I2CSlave slave\n");
	return -1;
    }
    return 0;
}

void
I2CSlave::address(int addr)
{
    _addr = addr;
    if (ioctl(i2c_handle, I2C_SLAVE_FORCE, addr) < 0) {
        fprintf(stderr, "Failed to set slave address %d\n", addr);
    }
}

void
I2CSlave::stop()
{
}
