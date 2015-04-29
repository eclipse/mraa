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

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "uart.h"
#include "mraa_internal.h"

mraa_uart_context
mraa_uart_init(int index)
{
    if (plat == NULL) {
        syslog(LOG_ERR, "uart: platform not initialised");
        return NULL;
    }

    if (advance_func->uart_init_pre != NULL) {
        if (advance_func->uart_init_pre(index) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "uart: failure in pre-init platform hook");
            return NULL;
        }
    }

    if (plat->uart_dev_count == 0) {
        syslog(LOG_ERR, "uart: platform has no UARTs defined");
        return NULL;
    }

    if (plat->uart_dev_count <= index) {
        syslog(LOG_ERR, "uart: platform has only %i", plat->uart_dev_count);
        return NULL;
    }

    int pos = plat->uart_dev[index].rx;
    if (pos >= 0) {
        if (plat->pins[pos].uart.mux_total > 0) {
            if (mraa_setup_mux_mapped(plat->pins[pos].uart) != MRAA_SUCCESS) {
                syslog(LOG_ERR, "uart: failed to setup muxes for RX pin");
                return NULL;
            }
        }
    }

    if (pos >= 0) {
        pos = plat->uart_dev[index].tx;
        if (plat->pins[pos].uart.mux_total > 0) {
            if (mraa_setup_mux_mapped(plat->pins[pos].uart) != MRAA_SUCCESS) {
                syslog(LOG_ERR, "uart: failed to setup muxes for TX pin");
                return NULL;
            }
        }
    }

    mraa_uart_context dev = (mraa_uart_context) malloc(sizeof(struct _uart));
    if (dev == NULL) {
        syslog(LOG_CRIT, "uart: Failed to allocate memory for context");
        return NULL;
    }
    memset(dev, 0, sizeof(struct _uart));

    dev->index = index;
    dev->fd = -1;
    dev->path = (char*) plat->uart_dev[index].device_path;
    if (advance_func->uart_init_post != NULL) {
        mraa_result_t ret = advance_func->uart_init_post(dev);
        if (ret != MRAA_SUCCESS) {
            free(dev);
            return NULL;
        }
    }

    return dev;
}

char*
mraa_uart_get_dev_path(mraa_uart_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "uart: get_device_path failed, context is NULL");
        return NULL;
    }
    if (dev->path == NULL) {
        syslog(LOG_ERR, "uart: device path undefined");
        return NULL;
    }
    return dev->path;
}
