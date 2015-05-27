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

#include "i2c.h"
#include "mraa_internal.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include "linux/i2c-dev.h"

// FIXME: Nasty macro to test for presence of function in context structure function table
#define IS_FUNC_DEFINED(dev, func)   (dev != NULL && dev->func_table != NULL && dev->func_table->func != NULL)


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


static mraa_adv_func_t* func_table;

int
mraa_i2c_smbus_access(int fh, uint8_t read_write, uint8_t command, int size, i2c_smbus_data_t* data)
{
    i2c_smbus_ioctl_data_t args;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;

    return ioctl(fh, I2C_SMBUS, &args);
}

mraa_i2c_context
mraa_i2c_init(int bus)
{
    mraa_board_t* board = plat;
    if (board == NULL) {
        syslog(LOG_ERR, "i2c: Platform Not Initialised");
        return NULL;
    }

    if (MRAA_IS_ON_SUB_PLATFORM(bus)) {
        syslog(LOG_NOTICE, "i2c: Using sub platform");
        board = board->sub_platform;
        if (board == NULL) {
            syslog(LOG_ERR, "i2c: Sub platform Not Initialised");
            return NULL;
        }
        bus = MRAA_GET_SUB_PLATFORM_INDEX(bus);
    }

    if (board->i2c_bus_count == 0) {
        syslog(LOG_ERR, "No i2c buses defined in platform");
        return NULL;
    }
    if (bus >= board->i2c_bus_count) {
        syslog(LOG_ERR, "Above i2c bus count");
        return NULL;
    }

    if (board->i2c_bus[bus].bus_id == -1) {
        syslog(LOG_ERR, "Invalid i2c bus, moving to default i2c bus");
        bus = board->def_i2c_bus;
    }

    int pos = board->i2c_bus[bus].sda;
    if (board->pins[pos].i2c.mux_total > 0) {
        if (mraa_setup_mux_mapped(board->pins[pos].i2c) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "i2c: Failed to set-up i2c sda multiplexer");
            return NULL;
        }
    }

    pos = board->i2c_bus[bus].scl;
    if (board->pins[pos].i2c.mux_total > 0) {
        if (mraa_setup_mux_mapped(board->pins[pos].i2c) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "i2c: Failed to set-up i2c scl multiplexer");
            return NULL;
        }
    }

    // FIXME: use global func_table variable to access advanced function table without changing i2c_init_raw() signature
    func_table = board->adv_func;
    return mraa_i2c_init_raw((unsigned int) board->i2c_bus[bus].bus_id);
}

mraa_i2c_context
mraa_i2c_init_raw(unsigned int bus)
{
    mraa_i2c_context dev = (mraa_i2c_context) malloc(sizeof(struct _i2c));
    if (dev == NULL) {
        syslog(LOG_CRIT, "i2c: Failed to allocate memory for context");
        return NULL;
    }
    
    dev->func_table = func_table;
    dev->busnum = bus;

    if (IS_FUNC_DEFINED(dev, i2c_init_pre)) {
        if (func_table->i2c_init_pre(bus) != MRAA_SUCCESS)
            return NULL;
    }
    if (!IS_FUNC_DEFINED(dev, i2c_init_bus_replace)) {
        char filepath[32];
        snprintf(filepath, 32, "/dev/i2c-%u", bus);
        if ((dev->fh = open(filepath, O_RDWR)) < 1) {
            syslog(LOG_ERR, "i2c: Failed to open requested i2c port %s", filepath);
        }
        syslog(LOG_DEBUG, "i2c: Opened i2c bus %s", filepath);

        if (ioctl(dev->fh, I2C_FUNCS, &dev->funcs) < 0) {
            syslog(LOG_CRIT, "i2c: Failed to get I2C_FUNC map from device");
            dev->funcs = 0;
        }
    } else {
        if (dev->func_table->i2c_init_bus_replace(dev) != MRAA_SUCCESS) {
            free(dev);
            return NULL;
        }
    }

    if (IS_FUNC_DEFINED(dev, i2c_init_post)) {
        mraa_result_t ret = dev->func_table->i2c_init_post(dev);
        if (ret != MRAA_SUCCESS) {
            free(dev);
            return NULL;
        }
    }
    return dev;
}

mraa_result_t
mraa_i2c_frequency(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    if (IS_FUNC_DEFINED(dev, i2c_set_frequency_replace)) {
        return dev->func_table->i2c_set_frequency_replace(dev, mode);
    }
    return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

int
mraa_i2c_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    int bytes_read = 0;
    if (!IS_FUNC_DEFINED(dev, i2c_read_replace)) {
        // this is the read(3) syscall not mraa_i2c_read()
        bytes_read = read(dev->fh, data, length);
    } else
        bytes_read = dev->func_table->i2c_read_replace(dev, data, length);
   if (bytes_read == length)
      return length;
   else
      return 0;
}

uint8_t
mraa_i2c_read_byte(mraa_i2c_context dev)
{
    i2c_smbus_data_t d;

    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_READ, I2C_NOCMD, I2C_SMBUS_BYTE, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return 0;
    }
    return 0x0FF & d.byte;
}

uint8_t
mraa_i2c_read_byte_data(mraa_i2c_context dev, uint8_t command)
{
    i2c_smbus_data_t d;

    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return 0;
    }
    return 0x0FF & d.byte;
}

uint16_t
mraa_i2c_read_word_data(mraa_i2c_context dev, uint8_t command)
{
    i2c_smbus_data_t d;

    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_READ, command, I2C_SMBUS_WORD_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return 0;
    }
    return 0xFFFF & d.word;
}

int
mraa_i2c_read_bytes_data(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
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

mraa_result_t
mraa_i2c_write(mraa_i2c_context dev, const uint8_t* data, int length)
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

mraa_result_t
mraa_i2c_write_byte(mraa_i2c_context dev, const uint8_t data)
{
    if (!IS_FUNC_DEFINED(dev, i2c_write_byte_replace)) {
        if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_WRITE, data, I2C_SMBUS_BYTE, NULL) < 0) {
            syslog(LOG_ERR, "i2c: Failed to write");
            return MRAA_ERROR_INVALID_HANDLE;
        }
        return MRAA_SUCCESS;
    } else
        return dev->func_table->i2c_write_byte_replace(dev, data);
}

mraa_result_t
mraa_i2c_write_byte_data(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    i2c_smbus_data_t d;

    d.byte = data;
    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_write_word_data(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    i2c_smbus_data_t d;

    d.word = data;
    if (mraa_i2c_smbus_access(dev->fh, I2C_SMBUS_WRITE, command, I2C_SMBUS_WORD_DATA, &d) < 0) {
        syslog(LOG_ERR, "i2c: Failed to write");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_i2c_address(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = (int) addr;
    if (dev->func_table->i2c_init_bus_replace == NULL) {
        if (ioctl(dev->fh, I2C_SLAVE_FORCE, addr) < 0) {
            syslog(LOG_ERR, "i2c: Failed to set slave address %d", addr);
            return MRAA_ERROR_INVALID_HANDLE;
        }
        return MRAA_SUCCESS;
    } else
        return dev->func_table->i2c_address_replace(dev, addr);
}

mraa_result_t
mraa_i2c_stop(mraa_i2c_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}
