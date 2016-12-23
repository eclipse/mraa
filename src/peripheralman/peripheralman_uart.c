/*
 * Author: Sanrio Alvares <sanrio.alvares@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

#include "uart.h"
#include "mraa_internal.h"
#include <ctype.h>
#include <string.h>

extern BPeripheralManagerClient *client;
extern char **uart_devices;
extern int uart_busses_count;

// do i need a speed(B9600) to int(9600) converter?

mraa_uart_context
mraa_uart_init_raw(const char* path)
{
    return NULL;
}

mraa_uart_context
mraa_uart_init(int index)
{
    int rc;
    char device_name[10] = {'\0'};
    mraa_uart_context dev;

    if (index > uart_busses_count) {
        return NULL;
    }

    dev = (mraa_uart_context) calloc(1, sizeof(struct _uart));
    if (dev == NULL) {
        return NULL;
    }

    snprintf(device_name, sizeof(device_name), "UART%d", index);

    rc = BPeripheralManagerClient_openUartDevice(client, device_name, &dev->buart);
    if (rc != 0) {
        BUartDevice_delete(dev->buart);
        free(dev);
        return NULL;
    }

    return dev;
}

mraa_result_t
mraa_uart_flush(mraa_uart_context dev)
{
    return -1;
}

mraa_result_t
mraa_uart_set_baudrate(mraa_uart_context dev, unsigned int baud)
{
    int rc;

    if (!dev) {
        syslog(LOG_ERR, "uart: stop: context is NULL");
        return 0;
    }

    rc = BUartDevice_setBaudrate(dev->buart, baud);
    if (rc != 0) {
        return 0;
    }

    return MRAA_SUCCESS;
}

int
mraa_uart_read(mraa_uart_context dev, char* buf, size_t length)
{
    int rc;
    uint32_t bytes_read;

    if (dev->buart == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = BUartDevice_read(dev->buart, buf, length, &bytes_read);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return bytes_read;
}

int
mraa_uart_write(mraa_uart_context dev, const char* buf, size_t length)
{
    int rc;
    uint32_t bytes_written;

    if (dev->buart == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = BUartDevice_write(dev->buart, buf, length, &bytes_written);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return bytes_written;
}

mraa_result_t
mraa_uart_set_mode(mraa_uart_context dev, int bytesize, mraa_uart_parity_t parity, int stopbits)
{
    return -1;
}

mraa_result_t
mraa_uart_set_flowcontrol(mraa_uart_context dev, mraa_boolean_t xonxoff, mraa_boolean_t rtscts)
{
    return -1;
}

mraa_result_t
mraa_uart_set_timeout(mraa_uart_context dev, int read, int write, int interchar)
{
    return -1;
}

mraa_result_t
mraa_uart_set_non_blocking(mraa_uart_context dev, mraa_boolean_t nonblock)
{
    return -1;
}

const char*
mraa_uart_get_dev_path(mraa_uart_context dev)
{
    return NULL;
}

mraa_result_t
mraa_uart_stop(mraa_uart_context dev)
{
    return -1;
}

mraa_boolean_t
mraa_uart_data_available(mraa_uart_context dev, unsigned int millis)
{
    return 0;
}
