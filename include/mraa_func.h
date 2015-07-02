/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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

#pragma once

#include "common.h"
#include "mraa.h"
#include "types.h"

typedef struct {
    mraa_i2c_context (*i2c_init_raw) (unsigned int bus);
    mraa_result_t (*i2c_frequency) (mraa_i2c_context dev, mraa_i2c_mode_t mode);
    mraa_result_t (*i2c_address) (mraa_i2c_context dev, uint8_t addr);
    int (*i2c_read) (mraa_i2c_context dev, uint8_t* data, int length);
    uint8_t (*i2c_read_byte) (mraa_i2c_context dev);
    uint8_t (*i2c_read_byte_data) (mraa_i2c_context dev, const uint8_t command);
    uint16_t (*i2c_read_word_data) (mraa_i2c_context dev, const uint8_t command);
    int (*i2c_read_bytes_data) (mraa_i2c_context dev, uint8_t command, uint8_t* data, int length);
    mraa_result_t (*i2c_write) (mraa_i2c_context dev, const uint8_t* data, int length);
    mraa_result_t (*i2c_write_byte) (mraa_i2c_context dev, const uint8_t data);
    mraa_result_t (*i2c_write_byte_data) (mraa_i2c_context dev, const uint8_t data, const uint8_t command);
    mraa_result_t (*i2c_write_word_data) (mraa_i2c_context dev, const uint16_t data, const uint8_t command);
    mraa_result_t (*i2c_stop) (mraa_i2c_context dev);
} mraa_i2c_func_t;


typedef struct {
    mraa_i2c_func_t* i2c;
} mraa_func_t;

