/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributions: Jon Trulson <jtrulson@ics.com>
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>

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

/**
 * Open the TTY device associated with a UART context, and set up the
 * terminal modes and baud rate.  The TTY is setup for a 'raw'
 * mode. 81N, no echo or special character handling, such as flow
 * control or line editing semantics.
 *
 * @param dev uart context
 * @param baud desired baud rate
 * @return mraa_result_t
 */
mraa_result_t mraa_uart_open_dev(mraa_uart_context dev, unsigned int baud);

/**
 * Close a device previously opened with mraa_uart_open_dev().
 *
 * @param dev uart context
 * @return mraa_result_t
 */
mraa_result_t mraa_uart_close_dev(mraa_uart_context dev);

/**
 * Read bytes from the device into a buffer
 *
 * @param dev uart context
 * @param buf buffer pointer
 * @param len maximum size of buffer
 * @return the number of bytes read, or -1 if an error occurred
 */
int mraa_uart_read(mraa_uart_context dev, char *buf, size_t len);

/**
 * Write bytes in buffer to a device
 *
 * @param dev uart context
 * @param buf buffer pointer
 * @param len maximum size of buffer
 * @return the number of bytes written, or -1 if an error occurred
 */
int mraa_uart_write(mraa_uart_context dev, char *buf, size_t len);

/**
 * Check to see if data is available on the device for reading
 *
 * @param dev uart context
 * @param millis number of milliseconds to wait, or 0 to return immediately
 * @return 1 if there is data available to read, 0 otherwise
 */
mraa_boolean_t
mraa_uart_data_available(mraa_uart_context dev, unsigned int millis);

#ifdef __cplusplus
}
#endif
