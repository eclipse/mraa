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
#include "mock/afb_board.h"

#define PLATFORM_NAME "MRAA AFB platform"

mraa_board_t*
mraa_mock_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    // Replace functions
    b->adv_func->i2c_init_bus_replace = &mraa_mock_i2c_init_bus_replace;
    b->adv_func->i2c_stop_replace = &mraa_mock_i2c_stop_replace;
    b->adv_func->i2c_set_frequency_replace = &mraa_mock_i2c_set_frequency_replace;
    b->adv_func->i2c_address_replace = &mraa_mock_i2c_address_replace;
    b->adv_func->i2c_read_replace = &mraa_mock_i2c_read_replace;
    b->adv_func->i2c_read_byte_replace = &mraa_mock_i2c_read_byte_replace;
    b->adv_func->i2c_read_byte_data_replace = &mraa_mock_i2c_read_byte_data_replace;
    b->adv_func->i2c_read_bytes_data_replace = &mraa_mock_i2c_read_bytes_data_replace;
    b->adv_func->i2c_read_word_data_replace = &mraa_mock_i2c_read_word_data_replace;
    b->adv_func->i2c_write_replace = &mraa_mock_i2c_write_replace;
    b->adv_func->i2c_write_byte_replace = &mraa_mock_i2c_write_byte_replace;
    b->adv_func->i2c_write_byte_data_replace = &mraa_mock_i2c_write_byte_data_replace;
    b->adv_func->i2c_write_word_data_replace = &mraa_mock_i2c_write_word_data_replace;

    return b;

error:
    syslog(LOG_CRIT, "MRAA mock: Platform failed to initialise");
    free(b);
    return NULL;
}
