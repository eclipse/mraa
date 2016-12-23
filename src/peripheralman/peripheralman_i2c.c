/*
 * Author: Constantin Musca <constantin.musca@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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
#include "mraa_internal.h"
#include <string.h>

extern BPeripheralManagerClient *client;
extern char **i2c_busses;
extern int i2c_busses_count;

mraa_i2c_context mraa_i2c_init_raw(unsigned int bus)
{
    mraa_i2c_context dev;

    if ((int)bus > i2c_busses_count) {
        return NULL;
    }

    dev = (mraa_i2c_context) calloc(1, sizeof(struct _i2c));
    if (dev == NULL) {
        syslog(LOG_CRIT, "i2c: Failed to allocate memory for context");
        return NULL;
    }

    dev->busnum = bus;
    return dev;
}

mraa_i2c_context mraa_i2c_init(int bus)
{
    return mraa_i2c_init_raw(bus);
}


mraa_result_t
mraa_i2c_frequency(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

int
mraa_i2c_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    int rc;

    if (dev->bi2c == NULL) {
        return 0;
    }

    rc = BI2cDevice_read(dev->bi2c, data, length);

    return rc;
}

int
mraa_i2c_read_byte(mraa_i2c_context dev)
{
    int rc;
    uint8_t val;

    if (dev->bi2c == NULL) {
        return 0;
    }

    rc = BI2cDevice_read(dev->bi2c, &val, 1);
    if (rc != 0 ) {
        return rc;
    }

    return val;
}

int
mraa_i2c_read_byte_data(mraa_i2c_context dev, uint8_t command)
{
    int rc;
    uint8_t val;

    if (dev->bi2c == NULL) {
        return 0;
    }

    rc = BI2cDevice_readRegByte(dev->bi2c, command, &val);
    if (rc != 0) {
        return 0;
    }

    return val;
}

int
mraa_i2c_read_word_data(mraa_i2c_context dev, uint8_t command)
{
    int rc;
    uint16_t val;

    if (dev->bi2c == NULL) {
        return 0;
    }

    rc = BI2cDevice_readRegWord(dev->bi2c, command, &val);
    if (rc != 0) {
        return 0;
    }

    return val;
}

int
mraa_i2c_read_bytes_data(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    int rc;

    if (dev->bi2c == NULL) {
        return -1;
    }

    rc = BI2cDevice_readRegBuffer(dev->bi2c, command, data, length);

    return rc;
}

mraa_result_t
mraa_i2c_write(mraa_i2c_context dev, const uint8_t* data, int length)
{
    int rc;

    if (dev->bi2c == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = BI2cDevice_write(dev->bi2c, data, length);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_write_byte(mraa_i2c_context dev, const uint8_t data)
{
    return mraa_i2c_write(dev, &data, 1);
}

mraa_result_t
mraa_i2c_write_byte_data(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    int rc;

    if (dev->bi2c == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = BI2cDevice_writeRegByte(dev->bi2c, command, data);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_write_word_data(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    int rc;

    if (dev->bi2c == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = BI2cDevice_writeRegWord(dev->bi2c, command, data);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_address(mraa_i2c_context dev, uint8_t addr)
{
    int rc;

    if (dev == NULL || dev->busnum > (int)i2c_busses_count) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    dev->addr = (int) addr;

    if (strlen(dev->bus_name) > 0) {
        rc = BPeripheralManagerClient_openI2cDevice(client,
            dev->bus_name, addr, &dev->bi2c);
    } else {
        rc = BPeripheralManagerClient_openI2cDevice(client,
            i2c_busses[dev->busnum], addr, &dev->bi2c);
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}


mraa_result_t
mraa_i2c_stop(mraa_i2c_context dev)
{
    if (dev->bi2c != NULL) {
        BI2cDevice_delete(dev->bi2c);
        dev->bi2c = NULL;
    }

    free(dev);
    return MRAA_SUCCESS;
}
