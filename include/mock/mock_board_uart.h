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

// ASCII code for "Z", used as a basis for our mock reads
#define MOCK_UART_DATA_BYTE 0x5A

mraa_result_t
mraa_mock_uart_set_baudrate_replace(mraa_uart_context dev, unsigned int baud);

mraa_result_t
mraa_mock_uart_init_raw_replace(mraa_uart_context dev, const char* path);

mraa_result_t
mraa_mock_uart_flush_replace(mraa_uart_context dev);

mraa_result_t
mraa_mock_uart_sendbreak_replace(mraa_uart_context dev, int duration);

mraa_result_t
mraa_mock_uart_set_flowcontrol_replace(mraa_uart_context dev, mraa_boolean_t xonxoff, mraa_boolean_t rtscts);

mraa_result_t
mraa_mock_uart_set_mode_replace(mraa_uart_context dev, int bytesize, mraa_uart_parity_t parity, int stopbits);

mraa_result_t
mraa_mock_uart_set_non_blocking_replace(mraa_uart_context dev, mraa_boolean_t nonblock);

mraa_result_t
mraa_mock_uart_set_timeout_replace(mraa_uart_context dev, int read, int write, int interchar);

mraa_boolean_t
mraa_mock_uart_data_available_replace(mraa_uart_context dev, unsigned int millis);

int
mraa_mock_uart_write_replace(mraa_uart_context dev, const char* buf, size_t len);

int
mraa_mock_uart_read_replace(mraa_uart_context dev, char* buf, size_t len);

#ifdef __cplusplus
}
#endif
