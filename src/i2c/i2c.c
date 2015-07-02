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
#include "mraa_func.h"

// FIXME: Nasty macro test for presence of function in function table
#define IS_FUNC_DEFINED(dev, func)   (dev != NULL && dev->func_table != NULL && dev->func_table->func != NULL)

extern mraa_func_t* mraa_drv_api_func_table;
static unsigned int drv_api;

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

    // FIXME: use global drv_type variable to access appropriate function table without changing i2c_init_raw() signature
    drv_api = plat->i2c_bus[bus].drv_type;
    int bus_index = bus && 0xFF;
    if (drv_api >= MRAA_DRV_API_MAX_ENTRIES) {
        syslog(LOG_ERR, "Invalid bus driver api");
        return NULL;
    }


    if (plat->i2c_bus[bus_index].bus_id == -1) {
        syslog(LOG_ERR, "Invalid i2c bus, moving to default i2c bus");
        bus = plat->def_i2c_bus;
    }

    int pos = plat->i2c_bus[bus_index].sda;
    if (plat->pins[pos].i2c.mux_total > 0) {
        if (mraa_setup_mux_mapped(plat->pins[pos].i2c) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "i2c: Failed to set-up i2c sda multiplexer");
            return NULL;
        }
    }

    pos = plat->i2c_bus[bus_index].scl;
    if (plat->pins[pos].i2c.mux_total > 0) {

        if (mraa_setup_mux_mapped(plat->pins[pos].i2c) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "i2c: Failed to set-up i2c scl multiplexer");
            return NULL;
        }
    }

    return mraa_i2c_init_raw((unsigned int) plat->i2c_bus[bus_index].bus_id);
}

mraa_i2c_context
mraa_i2c_init_raw(unsigned int bus)
{
    mraa_i2c_context context = NULL;
    mraa_i2c_func_t* func_table = mraa_drv_api_func_table[drv_api].i2c;        
    if (func_table != NULL && func_table->i2c_init_raw != NULL) {
        context = func_table->i2c_init_raw(bus);
        if (context != NULL)
            context->func_table = func_table;
    }
    return context;
}

mraa_result_t
mraa_i2c_frequency(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    if (IS_FUNC_DEFINED(dev, i2c_frequency))
        return dev->func_table->i2c_frequency(dev, mode);
    else
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

int
mraa_i2c_read(mraa_i2c_context dev, uint8_t* data, int length)
{
    if (IS_FUNC_DEFINED(dev, i2c_read))
        return dev->func_table->i2c_read(dev, data, length);
    else
      return 0;
}

uint8_t
mraa_i2c_read_byte(mraa_i2c_context dev)
{
    if (IS_FUNC_DEFINED(dev, i2c_read_byte))
    // if (dev->func_table != NULL && dev->func_table->i2c_read_byte != NULL)
        return dev->func_table->i2c_read_byte(dev);
    else
      return 0;
}

uint8_t
mraa_i2c_read_byte_data(mraa_i2c_context dev, uint8_t command)
{
    if (IS_FUNC_DEFINED(dev, i2c_read_byte_data))
    // if (dev->func_table != NULL && dev->func_table->i2c_read_byte_data != NULL)
        return dev->func_table->i2c_read_byte_data(dev, command);
    else
      return 0;
}

uint16_t
mraa_i2c_read_word_data(mraa_i2c_context dev, uint8_t command)
{
    if (IS_FUNC_DEFINED(dev, i2c_read_word_data))
    // if (dev->func_table != NULL && dev->func_table->i2c_read_word_data != NULL)
        return dev->func_table->i2c_read_word_data(dev, command);
    else
      return 0;
}

int
mraa_i2c_read_bytes_data(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    if (IS_FUNC_DEFINED(dev, i2c_read_bytes_data))
    // if (dev->func_table != NULL && dev->func_table->i2c_read_bytes_data != NULL)
        return dev->func_table->i2c_read_bytes_data(dev, command, data, length);
    else
      return -1;
}

mraa_result_t
mraa_i2c_write(mraa_i2c_context dev, const uint8_t* data, int length)
{
    if (IS_FUNC_DEFINED(dev, i2c_write))
    // if (dev->func_table != NULL && dev->func_table->i2c_write != NULL)
        return dev->func_table->i2c_write(dev, data, length);
    else
      return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

mraa_result_t
mraa_i2c_write_byte(mraa_i2c_context dev, const uint8_t data)
{
    if (IS_FUNC_DEFINED(dev, i2c_write_byte))
    // if (dev->func_table != NULL && dev->func_table->i2c_write_byte != NULL)
        return dev->func_table->i2c_write_byte(dev, data);
    else
      return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

mraa_result_t
mraa_i2c_write_byte_data(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    if (IS_FUNC_DEFINED(dev, i2c_write_byte_data))
    // if (dev->func_table != NULL && dev->func_table->i2c_write_byte_data != NULL)
        return dev->func_table->i2c_write_byte_data(dev, data, command);
    else
      return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

mraa_result_t
mraa_i2c_write_word_data(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    if (IS_FUNC_DEFINED(dev, i2c_write_word_data))
    // if (dev->func_table != NULL && dev->func_table->i2c_write_word_data != NULL)
        return dev->func_table->i2c_write_word_data(dev, data, command);
    else
      return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

mraa_result_t
mraa_i2c_address(mraa_i2c_context dev, uint8_t addr)
{
    if (IS_FUNC_DEFINED(dev, i2c_address))
    // if (dev->func_table != NULL && dev->func_table->i2c_address != NULL)
        return dev->func_table->i2c_address(dev, addr);
    else
      return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

mraa_result_t
mraa_i2c_stop(mraa_i2c_context dev)
{
    if (IS_FUNC_DEFINED(dev, i2c_write_byte_data))
    // if (dev->func_table != NULL && dev->func_table->i2c_stop != NULL)
        return dev->func_table->i2c_stop(dev);
    else
      return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

