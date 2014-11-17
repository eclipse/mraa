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
    if (plat == NULL) {
        syslog(LOG_ERR, "i2c: Platform Not Initialised");
        return NULL;
    }
    if (plat->i2c_bus_count == 0) {
        syslog(LOG_ERR, "No i2c buses defined in platform");
        return NULL;
    }
    if (bus >= plat->i2c_bus_count) {
        syslog(LOG_ERR, "Above i2c bus count");
        return NULL;
    }

    if (plat->i2c_bus[bus].bus_id == -1) {
        syslog(LOG_ERR, "Invalid i2c bus, moving to default i2c bus");
        bus = plat->def_i2c_bus;
    }

    int pos = plat->i2c_bus[bus].sda;
    if (plat->pins[pos].i2c.mux_total > 0)
        if (mraa_setup_mux_mapped(plat->pins[pos].i2c) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "i2c: Failed to set-up i2c sda multiplexer");
            return NULL;
        }

    pos = plat->i2c_bus[bus].scl;
    if (plat->pins[pos].i2c.mux_total > 0)
        if (mraa_setup_mux_mapped(plat->pins[pos].i2c) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "i2c: Failed to set-up i2c scl multiplexer");
            return NULL;
        }

    return mraa_i2c_init_raw((unsigned int) plat->i2c_bus[bus].bus_id);
}

mraa_i2c_context
mraa_i2c_init_raw(unsigned int bus)
{
    if (advance_func->i2c_init_pre != NULL) {
        if (advance_func->i2c_init_pre(bus) != MRAA_SUCCESS)
            return NULL;
    }
    mraa_i2c_context dev = (mraa_i2c_context) malloc(sizeof(struct _i2c));
    if (dev == NULL) {
        syslog(LOG_CRIT, "i2c: Failed to allocate memory for context");
        return NULL;
    }

    char filepath[32];
    snprintf(filepath, 32, "/dev/i2c-%u", bus);
    if ((dev->fh = open(filepath, O_RDWR)) < 1) {
        syslog(LOG_ERR, "i2c: Failed to open requested i2c port %s", filepath);
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

    return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
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
    return byte;
}

uint8_t
mraa_i2c_read_byte_data(mraa_i2c_context dev, uint8_t command)
{
    return (uint8_t) i2c_smbus_read_byte_data(dev->fh, command);
}

uint16_t
mraa_i2c_read_word_data(mraa_i2c_context dev, uint8_t command)
{
    return (uint16_t) i2c_smbus_read_word_data(dev->fh, command);
}

mraa_result_t
mraa_i2c_write(mraa_i2c_context dev, const uint8_t* data, int length)
{
    if (i2c_smbus_write_i2c_block_data(dev->fh, data[0], length-1, (uint8_t*) data+1) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_write_byte(mraa_i2c_context dev, const uint8_t data)
{
    if (i2c_smbus_write_byte(dev->fh, data) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_write_byte_data(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    if (i2c_smbus_write_byte_data(dev->fh, command, data) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_write_word_data(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    if (i2c_smbus_write_word_data(dev->fh, command, data) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_address(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = (int) addr;
    if (ioctl(dev->fh, I2C_SLAVE_FORCE, addr) < 0) {
        syslog(LOG_ERR, "i2c: Failed to set slave address %d", addr);
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
