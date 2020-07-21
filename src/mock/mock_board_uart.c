/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "mock/mock_board_uart.h"

mraa_result_t
mraa_mock_uart_set_baudrate_replace(mraa_uart_context dev, unsigned int baud)
{
    // Limits are taken from uart.c::uint2speed(), they don't matter much anyway
    if ((baud < 0) || (baud > 4000000)) {
        syslog(LOG_ERR, "uart%i: set_baudrate: invalid baudrate: %i", dev->index, baud);
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_uart_init_raw_replace(mraa_uart_context dev, const char* path)
{
    // The only thing we have to do from the original uart_init_raw()
    return mraa_uart_set_baudrate(dev, 9600);
}

mraa_result_t
mraa_mock_uart_flush_replace(mraa_uart_context dev)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_uart_sendbreak_replace(mraa_uart_context dev, int duration)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_uart_set_flowcontrol_replace(mraa_uart_context dev, mraa_boolean_t xonxoff, mraa_boolean_t rtscts)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_uart_set_mode_replace(mraa_uart_context dev, int bytesize, mraa_uart_parity_t parity, int stopbits)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_uart_set_non_blocking_replace(mraa_uart_context dev, mraa_boolean_t nonblock)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_uart_set_timeout_replace(mraa_uart_context dev, int read, int write, int interchar)
{
    return MRAA_SUCCESS;
}

mraa_boolean_t
mraa_mock_uart_data_available_replace(mraa_uart_context dev, unsigned int millis)
{
    // Our mock implementation will always have "incoming" data
    return 1;
}

int
mraa_mock_uart_write_replace(mraa_uart_context dev, const char* buf, size_t len)
{
    // Our mock implementation always succeeds when sending data
    return len;
}

int
mraa_mock_uart_read_replace(mraa_uart_context dev, char* buf, size_t len)
{
    // We'll return MOCK_UART_DATA_BYTE, len times
    memset(buf, MOCK_UART_DATA_BYTE, len);
    return len;
}
