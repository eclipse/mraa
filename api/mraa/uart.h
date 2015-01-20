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

/**
 * @file
 * @brief UART module
 *
 * UART is the Universal asynchronous receiver/transmitter interface to
 * libmraa. It allows the exposure of UART pins on supported boards.
 * With functionality to expand at a later date.
 *
 * @snippet uart_setup.c Interesting
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <fcntl.h>

#include "common.h"

typedef struct _uart* mraa_uart_context;

/**
 * Initialise uart_context, uses board mapping
 *
 * @param uart the index of the uart set to use
 * @return uart context or NULL
 */
mraa_uart_context mraa_uart_init(int uart);

/**
 * Get Char pointer with tty device path within Linux
 * For example. Could point to "/dev/ttyS0"
 *
 * @param dev uart context
 * @return char pointer of device path
 */
char* mraa_uart_get_dev_path(mraa_uart_context dev);

#ifdef __cplusplus
}
#endif
