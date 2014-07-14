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

#include "i2c.h"
#include "smbus.h"
#include "mraa_internal.h"

mraa_i2c_context
mraa_i2c_init(int bus)
{
    int checked_pin = mraa_setup_i2c(bus);
    if (checked_pin < 0) {
        switch(checked_pin) {
            case -1:
                fprintf(stderr, "No i2c on board\n");
                return NULL;
            case -2:
                fprintf(stderr, "Failed to set-up i2c multiplexer!\n");
                return NULL;
            case -3:
                fprintf(stderr, "Platform Not Initialised\n");
                return NULL;
            default: return NULL;
        }
    }
    return mraa_i2c_init_raw((unsigned int) checked_pin);
}

mraa_i2c_context
mraa_i2c_init_raw(unsigned int bus)
{
    if (advance_func->i2c_init_pre != NULL) {
        if (advance_func->i2c_init_pre(bus) != MRAA_SUCCESS)
            return NULL;
    }
    mraa_i2c_context dev = (mraa_i2c_context) malloc(sizeof(struct _i2c));
    if (dev == NULL)
        return NULL;

    char filepath[32];
    snprintf(filepath, 32, "/dev/i2c-%u", bus);
    if ((dev->fh = open(filepath, O_RDWR)) < 1) {
        fprintf(stderr, "Failed to open requested i2c port %s\n", filepath);
    }

    if (advance_func->i2c_init_post != NULL) {
        mraa_result_t ret = advance_func->i2c_init_post(dev);
        if (ret != MRAA_SUCCESS) {
            free(dev);
            return NULL;
        }
    }
    return dev;
}

mraa_result_t
mraa_i2c_frequency(mraa_i2c_context dev, int hz)
{
    dev->hz = hz;

    return MRAA_SUCCESS;
}

int
mraa_i2c_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    // this is the read(3) syscall not mraa_i2c_read()
    if (read(dev->fh, data, length) == length) {
        return length;
    }
    return 0;
}

uint8_t
mraa_i2c_read_byte(mraa_i2c_context dev)
{
    uint8_t byte = i2c_smbus_read_byte(dev->fh);
    if (byte < 0) {
        return -1;
    }
    return byte;
}

mraa_result_t
mraa_i2c_write(mraa_i2c_context dev, const uint8_t* data, int length)
{
    if (i2c_smbus_write_i2c_block_data(dev->fh, data[0], length-1, (uint8_t*) data+1) < 0) {
        fprintf(stderr, "Failed to write to i2c\n");
	return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_write_byte(mraa_i2c_context dev, const uint8_t data)
{
    if (i2c_smbus_write_byte(dev->fh, data) < 0) {
        fprintf(stderr, "Failed to write to i2c\n");
	return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_address(mraa_i2c_context dev, int addr)
{
    dev->addr = addr;
    if (ioctl(dev->fh, I2C_SLAVE_FORCE, addr) < 0) {
        fprintf(stderr, "Failed to set slave address %d\n", addr);
	return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_stop(mraa_i2c_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}
