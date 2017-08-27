/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2017 Intel Corp.
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
#include <string.h>

#include "common.h"
#include "afb/afb_board.h"

#define PLATFORM_NAME "MRAA AFB platform"

mraa_result_t
mraa_afb_i2c_init_bus_replace(mraa_i2c_context dev)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_stop_replace(mraa_i2c_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_set_frequency_replace(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    return MRAA_ERROR_INVALID_PARAMETER;
}

mraa_result_t
mraa_afb_i2c_address_replace(mraa_i2c_context dev, uint8_t addr)
{
    return MRAA_SUCCESS;
}

int
mraa_afb_i2c_read_replace(mraa_i2c_context dev, uint8_t* data, int length)
{
    return -1;
}

int
mraa_afb_i2c_read_byte_replace(mraa_i2c_context dev)
{
    return -1;
}

int
mraa_afb_i2c_read_byte_data_replace(mraa_i2c_context dev, uint8_t command)
{
    return -1;
}

int
mraa_afb_i2c_read_bytes_data_replace(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    return -1;
}

int
mraa_afb_i2c_read_word_data_replace(mraa_i2c_context dev, uint8_t command)
{
    return -1;
}

mraa_result_t
mraa_afb_i2c_write_replace(mraa_i2c_context dev, const uint8_t* data, int length)
{
    return MRAA_ERROR_UNSPECIFIED;
}

mraa_result_t
mraa_afb_i2c_write_byte_replace(mraa_i2c_context dev, const uint8_t data)
{
    return MRAA_ERROR_UNSPECIFIED;
}

mraa_result_t
mraa_afb_i2c_write_byte_data_replace(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    return MRAA_ERROR_UNSPECIFIED;
}

mraa_result_t
mraa_afb_i2c_write_word_data_replace(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    return MRAA_ERROR_UNSPECIFIED;
}

mraa_board_t*
mraa_afb_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }
    b->platform_name = PLATFORM_NAME;

    // Replace functions
    b->adv_func->i2c_init_bus_replace = &mraa_afb_i2c_init_bus_replace;
    b->adv_func->i2c_stop_replace = &mraa_afb_i2c_stop_replace;
    b->adv_func->i2c_set_frequency_replace = &mraa_afb_i2c_set_frequency_replace;
    b->adv_func->i2c_address_replace = &mraa_afb_i2c_address_replace;
    b->adv_func->i2c_read_replace = &mraa_afb_i2c_read_replace;
    b->adv_func->i2c_read_byte_replace = &mraa_afb_i2c_read_byte_replace;
    b->adv_func->i2c_read_byte_data_replace = &mraa_afb_i2c_read_byte_data_replace;
    b->adv_func->i2c_read_bytes_data_replace = &mraa_afb_i2c_read_bytes_data_replace;
    b->adv_func->i2c_read_word_data_replace = &mraa_afb_i2c_read_word_data_replace;
    b->adv_func->i2c_write_replace = &mraa_afb_i2c_write_replace;
    b->adv_func->i2c_write_byte_replace = &mraa_afb_i2c_write_byte_replace;
    b->adv_func->i2c_write_byte_data_replace = &mraa_afb_i2c_write_byte_data_replace;
    b->adv_func->i2c_write_word_data_replace = &mraa_afb_i2c_write_word_data_replace;

    return b;

error:
    syslog(LOG_CRIT, "MRAA afb: Platform failed to initialise");
    free(b);
    return NULL;
}

mraa_platform_t
mraa_afb_platform()
{
    plat = mraa_afb_board();

    return MRAA_AFB_PLATFORM;
}
