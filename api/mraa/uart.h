/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributions: Jon Trulson <jtrulson@ics.com>
 *                Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 - 2015 Intel Corporation
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
 *
 * SPDX-License-Identifier: MIT
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
 * @snippet uart.c Interesting
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "common.h"

/** Mraa Uart Context */
typedef struct _uart* mraa_uart_context;

/**
 * Initialise uart_context, uses board mapping
 *
 * @param uart the index of the uart set to use
 * @return uart context or NULL
 */
mraa_uart_context mraa_uart_init(int uart);

/**
 * Initialise a raw uart_context. No board setup.
 *
 * @param path for example "/dev/ttyS0"
 * @return uart context or NULL
 */
mraa_uart_context mraa_uart_init_raw(const char* path);

/**
 * Flush the outbound data.
 * Blocks until complete.
 *
 * @param dev The UART context
 * @return Result of operation
 */
mraa_result_t mraa_uart_flush(mraa_uart_context dev);

/**
 * Send a break to the device.
 * Blocks until complete.
 *
 * @param dev The UART context
 * @param duration When 0, send a break lasting at least 250
 * milliseconds, and not more than 500 milliseconds.  When non zero,
 * the break duration is implementation specific.
 * @return Result of operation
 */
mraa_result_t mraa_uart_sendbreak(mraa_uart_context dev, int duration);

/**
 * Set the baudrate.
 * Takes an int and will attempt to decide what baudrate  is
 * to be used on the UART hardware.
 *
 * @param dev The UART context
 * @param baud unsigned int of baudrate i.e. 9600
 * @return Result of operation
 */
mraa_result_t mraa_uart_set_baudrate(mraa_uart_context dev, unsigned int baud);

/**
 * Set the transfer mode
 * For example setting the mode to 8N1 would be
 * "mraa_uart_set_mode(dev, 8,MRAA_UART_PARITY_NONE , 1)"
 *
 * @param dev The UART context
 * @param bytesize data bits
 * @param parity Parity bit setting
 * @param stopbits stop bits
 * @return Result of operation
 */
mraa_result_t mraa_uart_set_mode(mraa_uart_context dev, int bytesize, mraa_uart_parity_t parity, int stopbits);

/**
 * Set the flowcontrol
 *
 * @param dev The UART context
 * @param xonxoff XON/XOFF Software flow control.
 * @param rtscts RTS/CTS out of band hardware flow control
 * @return Result of operation
 */
mraa_result_t mraa_uart_set_flowcontrol(mraa_uart_context dev, mraa_boolean_t xonxoff, mraa_boolean_t rtscts);

/**
 * Set the timeout for read and write operations
 * <= 0 will disable that timeout
 *
 * @param dev The UART context
 * @param read read timeout
 * @param write write timeout
 * @param interchar inbetween char timeout
 * @return Result of operation
 */
mraa_result_t mraa_uart_set_timeout(mraa_uart_context dev, int read, int write, int interchar);

/**
 * Set the blocking state for write operations
 *
 * @param dev The UART context
 * @param nonblock new nonblocking state
 * @return Result of operation
 */
mraa_result_t mraa_uart_set_non_blocking(mraa_uart_context dev, mraa_boolean_t nonblock);

/**
 * Get Char pointer with tty device path within Linux
 * For example. Could point to "/dev/ttyS0"
 *
 * @param dev uart context
 * @return char pointer of device path
 */
const char* mraa_uart_get_dev_path(mraa_uart_context dev);

/**
 * Get the current settings of an UART. This is an unintrusive function. Meaning
 * it intends not to change anything, only read the values without disturbing.
 *
 * All but the first index parameter are "outparameters". That means they can
 * contain values on return. If any parameter is not interesting, a null pointer
 * can be sent instead as a placeholder.
 * The devpath parameter can be either in or out parameter. In case of a negative
 * index,  the UART is identified using *devpath instead. This functionality is
 * intended for and needed by for instance USB serial adapters.
 *
 * In case of a non-success return value, the outparameters are undefined.
 *
 * @param index uart index to look up, if negative, *devpath will be used instead
 * @param devpath points to the device path of the UART, eg: /dev/ttyS0
 * @param name outparameter that on return will point to the name of the UART
 * @param baudrate pointer to an integer to contain the current baudrate (0--4M)
 * @param databits pointer to an integer to contain the number databits (5--8)
 * @param stopbits pointer to an integer to contain the number stopbits (1--2)
 * @param parity will contain the current parity mode
 * @param rtscts will point to non-zero if CTS/RTS flow control is enabled, zero otherwise
 * @param xonxoff will point to a non-zero value if xon/xoff flow control is enabled
 * @return result
 */
mraa_result_t
mraa_uart_settings(int index,
    const char **devpath,
    const char **name,
    int* baudrate,
    int* databits,
    int* stopbits,
    mraa_uart_parity_t* parity,
    unsigned int* rtscts,
    unsigned int* xonxoff);

/**
 * Destroy a mraa_uart_context
 *
 * @param dev uart context
 * @return mraa_result_t
 */
mraa_result_t mraa_uart_stop(mraa_uart_context dev);

/**
 * Read bytes from the device into a buffer
 *
 * @param dev uart context
 * @param buf buffer pointer
 * @param length maximum size of buffer
 * @return the number of bytes read, or -1 if an error occurred
 */
int mraa_uart_read(mraa_uart_context dev, char* buf, size_t length);

/**
 * Write bytes in buffer to a device
 *
 * @param dev uart context
 * @param buf buffer pointer
 * @param length maximum size of buffer
 * @return the number of bytes written, or -1 if an error occurred
 */
int mraa_uart_write(mraa_uart_context dev, const char* buf, size_t length);

/**
 * Check to see if data is available on the device for reading
 *
 * @param dev uart context
 * @param millis number of milliseconds to wait, or 0 to return immediately
 * @return 1 if there is data available to read, 0 otherwise
 */
mraa_boolean_t mraa_uart_data_available(mraa_uart_context dev, unsigned int millis);

#ifdef __cplusplus
}
#endif
