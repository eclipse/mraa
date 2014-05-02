/*
 * Author: Brendan Le Foll
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

maa_i2c_context*
maa_i2c_init(int bus)
{
    unsigned int checked_pin = maa_check_i2c(bus);
    if (checked_pin < 0) {
        switch(checked_pin) {
            case -1:
                fprintf(stderr, "No i2c on board\n");
                return NULL;
            case -2:
                fprintf(stderr, "Failed to set-up i2c multiplexer!\n");
                return NULL;
            case -3:
                fprintf(stderr, "Platform Not Initialised");
                return NULL;
            default: return NULL;
        }
    }
    return maa_i2c_init_raw(checked_pin);
}

maa_i2c_context*
maa_i2c_init_raw(unsigned int bus)
{
    if (bus < 0) {
        fprintf(stderr, "Bus -%u- bellow zero\n", bus);
        return NULL;
    }
    maa_i2c_context* dev = (maa_i2c_context*) malloc(sizeof(maa_i2c_context));
    if (dev == NULL)
        return NULL;

    char filepath[32];
    snprintf(filepath, 32, "/dev/i2c-%u", bus);
    if ((dev->fh = open(filepath, O_RDWR)) < 1) {
        fprintf(stderr, "Failed to open requested i2c port %s", filepath);
    }
    return dev;
}

maa_result_t
maa_i2c_frequency(maa_i2c_context* dev, int hz)
{
    dev->hz = hz;

    return MAA_SUCCESS;
}

maa_result_t
maa_i2c_read(maa_i2c_context* dev, char *data, int length)
{
    // this is the read(3) syscall not maa_i2c_read()
    if (read(dev->fh, data, length) == length) {
        return length;
    }
    return MAA_ERROR_NO_DATA_AVAILABLE;
}

int
maa_i2c_read_byte(maa_i2c_context* dev)
{
    int byte;
    byte = i2c_smbus_read_byte(dev->fh);
    if (byte < 0) {
        return -1;
    }
    return byte;
}

maa_result_t
maa_i2c_write(maa_i2c_context* dev, const char* data, int length)
{
    if (i2c_smbus_write_i2c_block_data(dev->fh, data[0], length-1, (uint8_t*) data+1) < 0) {
        fprintf(stderr, "Failed to write to i2c\n");
	return MAA_ERROR_INVALID_HANDLE;
    }
    return MAA_SUCCESS;
}

maa_result_t
maa_i2c_write_byte(maa_i2c_context* dev, int data)
{
    if (i2c_smbus_write_byte(dev->fh, data) < 0) {
        fprintf(stderr, "Failed to write to i2c\n");
	return MAA_ERROR_INVALID_HANDLE;
    }
    return MAA_SUCCESS;
}

maa_result_t
maa_i2c_address(maa_i2c_context* dev, int addr)
{
    dev->addr = addr;
    if (ioctl(dev->fh, I2C_SLAVE_FORCE, addr) < 0) {
        fprintf(stderr, "Failed to set slave address %d\n", addr);
	return MAA_ERROR_INVALID_HANDLE;
    }
    return MAA_SUCCESS;
}

maa_result_t
maa_i2c_stop(maa_i2c_context* dev)
{
    free(dev);
    return MAA_SUCCESS;
}
