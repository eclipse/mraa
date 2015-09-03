/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
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

#pragma once

#include "common.h"
#include "mraa.h"
#include "types.h"

// FIXME: Nasty macro to test for presence of function in context structure function table
#define IS_FUNC_DEFINED(dev, func)   (dev != NULL && dev->advance_func != NULL && dev->advance_func->func != NULL)

typedef struct {
    mraa_result_t (*gpio_init_internal_replace) (int pin);
    mraa_result_t (*gpio_init_pre) (int pin);
    mraa_result_t (*gpio_init_post) (mraa_gpio_context dev);

    mraa_result_t (*gpio_close_pre) (mraa_gpio_context dev);

    mraa_result_t (*gpio_mode_replace) (mraa_gpio_context dev, mraa_gpio_mode_t mode);
    mraa_result_t (*gpio_mode_pre) (mraa_gpio_context dev, mraa_gpio_mode_t mode);
    mraa_result_t (*gpio_mode_post) (mraa_gpio_context dev, mraa_gpio_mode_t mode);
    
    mraa_result_t (*gpio_edge_mode_replace) (mraa_gpio_context dev, mraa_gpio_edge_t mode);

    mraa_result_t (*gpio_dir_replace) (mraa_gpio_context dev, mraa_gpio_dir_t dir);
    mraa_result_t (*gpio_dir_pre) (mraa_gpio_context dev, mraa_gpio_dir_t dir);
    mraa_result_t (*gpio_dir_post) (mraa_gpio_context dev, mraa_gpio_dir_t dir);

    int (*gpio_read_replace) (mraa_gpio_context dev);
    mraa_result_t (*gpio_write_replace) (mraa_gpio_context dev, int value);
    mraa_result_t (*gpio_write_pre) (mraa_gpio_context dev, int value);
    mraa_result_t (*gpio_write_post) (mraa_gpio_context dev, int value);
    mraa_result_t (*gpio_mmap_setup) (mraa_gpio_context dev, mraa_boolean_t en);
    void* (*gpio_interrupt_handler_replace) (mraa_gpio_context dev); 

    mraa_result_t (*i2c_init_pre) (unsigned int bus);
    mraa_result_t (*i2c_init_bus_replace) (mraa_i2c_context dev);
    mraa_i2c_context (*i2c_init_raw_replace) (unsigned int bus);
    mraa_result_t (*i2c_init_post) (mraa_i2c_context dev);
    mraa_result_t (*i2c_set_frequency_replace) (mraa_i2c_context dev, mraa_i2c_mode_t mode);
    mraa_result_t (*i2c_address_replace) (mraa_i2c_context dev, uint8_t addr);
    int (*i2c_read_replace) (mraa_i2c_context dev, uint8_t* data, int length);
    uint8_t (*i2c_read_byte_replace) (mraa_i2c_context dev);
    uint8_t (*i2c_read_byte_data_replace) (mraa_i2c_context dev, const uint8_t command);
    uint16_t (*i2c_read_word_data_replace) (mraa_i2c_context dev, const uint8_t command);
    int (*i2c_read_bytes_data_replace) (mraa_i2c_context dev, uint8_t command, uint8_t* data, int length);
    mraa_result_t (*i2c_write_replace) (mraa_i2c_context dev, const uint8_t* data, int length);
    mraa_result_t (*i2c_write_byte_replace) (mraa_i2c_context dev, uint8_t data);
    mraa_result_t (*i2c_write_byte_data_replace) (mraa_i2c_context dev, const uint8_t data, const uint8_t command);
    mraa_result_t (*i2c_write_word_data_replace) (mraa_i2c_context dev, const uint16_t data, const uint8_t command);
    mraa_result_t (*i2c_stop_replace) (mraa_i2c_context dev);

    mraa_result_t (*aio_get_valid_fp) (mraa_aio_context dev);
    mraa_result_t (*aio_init_pre) (unsigned int aio);
    mraa_result_t (*aio_init_post) (mraa_aio_context dev);

    mraa_pwm_context (*pwm_init_replace) (int pin);
    mraa_result_t (*pwm_init_pre) (int pin);
    mraa_result_t (*pwm_init_post) (mraa_pwm_context pwm);
    mraa_result_t (*pwm_period_replace) (mraa_pwm_context dev, int period);

    mraa_result_t (*spi_init_pre) (int bus);
    mraa_result_t (*spi_init_post) (mraa_spi_context spi);
    mraa_result_t (*spi_lsbmode_replace) (mraa_spi_context dev, mraa_boolean_t lsb);

    mraa_result_t (*uart_init_pre) (int index);
    mraa_result_t (*uart_init_post) (mraa_uart_context uart);
} mraa_adv_func_t;
