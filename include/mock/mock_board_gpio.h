/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

mraa_result_t
mraa_mock_gpio_init_internal_replace(mraa_gpio_context dev, int pin);

mraa_result_t
mraa_mock_gpio_close_replace(mraa_gpio_context dev);

mraa_result_t
mraa_mock_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir);

mraa_result_t
mraa_mock_gpio_read_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t* dir);

int
mraa_mock_gpio_read_replace(mraa_gpio_context dev);

mraa_result_t
mraa_mock_gpio_write_replace(mraa_gpio_context dev, int value);

mraa_result_t
mraa_mock_gpio_edge_mode_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode);

mraa_result_t
mraa_mock_gpio_isr_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode, void (*fptr)(void*), void* args);

mraa_result_t
mraa_mock_gpio_isr_exit_replace(mraa_gpio_context dev);

mraa_result_t
mraa_mock_gpio_mode_replace(mraa_gpio_context dev, mraa_gpio_mode_t mode);

#ifdef __cplusplus
}
#endif
