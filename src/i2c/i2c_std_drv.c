/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Code is modified from the RoadNarrows-robotics i2c library (distributed under
 * the MIT license, license is included verbatim under src/i2c/LICENSE)
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

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "linux/i2c-dev.h"
#include "mraa_internal.h"
#include "i2c.h"

typedef union i2c_smbus_data_union {
    uint8_t byte;        ///< data byte
    unsigned short word; ///< data short word
    uint8_t block[I2C_SMBUS_BLOCK_MAX + 2];
    ///< block[0] is used for length and one more for PEC
} i2c_smbus_data_t;

typedef struct i2c_smbus_ioctl_data_struct {
    uint8_t read_write;     ///< operation direction
    uint8_t command;        ///< ioctl command
    int size;               ///< data size
    i2c_smbus_data_t* data; ///< data
} i2c_smbus_ioctl_data_t;

static int
mraa_i2c_smbus_access(int fh, uint8_t read_write, uint8_t command, int size, i2c_smbus_data_t* data)
{
    i2c_smbus_ioctl_data_t args;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;

    return ioctl(fh, I2C_SMBUS, &args);
}

static mraa_i2c_context
mraa_i2c_drv_init_raw(unsigned int bus)
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

    dev->busnum = bus;
    char filepath[32];
    snprintf(filepath, 32, "/dev/i2c-%u", bus);
    if ((dev->fh = open(filepath, O_RDWR)) < 1) {
        syslog(LOG_ERR, "i2c: Failed to open requested i2c port %s", filepath);
    }

    if (ioctl(dev->fh, I2C_FUNCS, &dev->funcs) < 0) {
        syslog(LOG_CRIT, "i2c: Failed to get I2C_FUNC map from device");
        dev->funcs = 0;
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

static mraa_result_t
mraa_i2c_drv_frequency(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    if (advance_func->i2c_set_frequency_replace != NULL) {
        return advance_func->i2c_set_frequency_replace(dev, mode);
    }
    return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

static int
mraa_i2c_drv_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    int bytes_read = advance_func->i2c_read_replace(dev, data, length);
   if (bytes_read == length)
      return length;
   else
      return 0;
}

static uint8_t
mraa_i2c_drv_read_byte(mraa_i2c_context dev)
{
    i2c_smbus_data_t d;

    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_READ, I2C_NOCMD, I2C_SMBUS_BYTE, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return 0;
    }
    return 0x0FF & d.byte;
}

static uint8_t
mraa_i2c_drv_read_byte_data(mraa_i2c_context dev, uint8_t command)
{
    i2c_smbus_data_t d;

    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return 0;
    }
    return 0x0FF & d.byte;
}

static uint16_t
mraa_i2c_drv_read_word_data(mraa_i2c_context dev, uint8_t command)
{
    i2c_smbus_data_t d;

    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_READ, command, I2C_SMBUS_WORD_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return 0;
    }
    return 0xFFFF & d.word;
}

static int
mraa_i2c_drv_read_bytes_data(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    struct i2c_rdwr_ioctl_data d;
    struct i2c_msg m[2];

    m[0].addr = dev->addr;
    m[0].flags = 0x00;
    m[0].len = 1;
    m[0].buf = &command;
    m[1].addr = dev->addr;
    m[1].flags = I2C_M_RD;
    m[1].len = length;
    m[1].buf = data;

    d.msgs = m;
    d.nmsgs = 2;

    return ioctl(dev->fh, I2C_RDWR, &d) < 0 ? -1 : length;
}

static mraa_result_t
mraa_i2c_drv_write(mraa_i2c_context dev, const uint8_t* data, int length)
{
    i2c_smbus_data_t d;
    int i;
    uint8_t command = data[0];

    data = &data[1];
    length = length - 1;
    if (length > I2C_SMBUS_I2C_BLOCK_MAX) {
        length = I2C_SMBUS_I2C_BLOCK_MAX;
    }

    for (i = 1; i <= length; i++) {
        d.block[i] = data[i - 1];
    }
    d.block[0] = length;

    return mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_WRITE, command, I2C_SMBUS_I2C_BLOCK_DATA, &d);
}

static mraa_result_t
mraa_i2c_drv_write_byte(mraa_i2c_context dev, const uint8_t data)
{
    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_WRITE, data, I2C_SMBUS_BYTE, NULL) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_i2c_drv_write_byte_data(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    i2c_smbus_data_t d;

    d.byte = data;
    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_i2c_drv_write_word_data(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    i2c_smbus_data_t d;

    d.word = data;
    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_WRITE, command, I2C_SMBUS_WORD_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_i2c_drv_address(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = (int) addr;
    if (ioctl(dev->fh, I2C_SLAVE_FORCE, addr) < 0) {
        syslog(LOG_ERR, "i2c: Failed to set slave address %d", addr);
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_i2c_drv_stop(mraa_i2c_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}

mraa_i2c_func_t*
mraa_i2c_drv_create_func_table()
{
    mraa_i2c_func_t* func_table = (mraa_i2c_func_t*) calloc(1, sizeof(mraa_i2c_func_t));
    if (func_table != NULL) {
        func_table->i2c_init_raw = &mraa_i2c_drv_init_raw;
        func_table->i2c_frequency = &mraa_i2c_drv_frequency;
        func_table->i2c_address = &mraa_i2c_drv_address;
        func_table->i2c_read = &mraa_i2c_drv_read;
        func_table->i2c_read_byte = &mraa_i2c_drv_read_byte;
        func_table->i2c_read_byte_data = &mraa_i2c_drv_read_byte_data;
        func_table->i2c_read_word_data = &mraa_i2c_drv_read_word_data;
        func_table->i2c_read_bytes_data = &mraa_i2c_drv_read_bytes_data;
        func_table->i2c_write = &mraa_i2c_drv_write;
        func_table->i2c_write_byte = &mraa_i2c_drv_write_byte;
        func_table->i2c_write_byte_data = &mraa_i2c_drv_write_byte_data;
        func_table->i2c_write_word_data = &mraa_i2c_drv_write_word_data;
        func_table->i2c_stop = &mraa_i2c_drv_stop;    
    }
    return func_table;
}

