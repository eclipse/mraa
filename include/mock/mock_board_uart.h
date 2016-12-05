/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
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
