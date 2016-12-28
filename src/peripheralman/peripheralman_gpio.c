/*
 * Author: Constantin Musca <constantin.musca@intel.com>
 *         Brendan Le Foll <brendan.le.foll@intel.com>
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

#include <stdlib.h>
#include <peripheralmanager/peripheral_manager_client.h>
#include <peripheralmanager/gpio.h>

// API
#include "mraa/gpio.h"

// mraa internals
#include "mraa_internal.h"

extern BPeripheralManagerClient *client;
extern char **gpios;
extern int gpios_count;

static mraa_result_t
mraa_pman_gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    rc = BPeripheralManagerClient_openGpio(client, gpios[pin], &dev->bgpio);
    if (rc != 0) {
        syslog(LOG_ERROR, "peripheralmanager: Failed to init gpio");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    dev->pin = pin;
    dev->phy_pin = pin;

    return dev;
}

static mraa_result_t
mraa_pman_gpio_close_replace(mraa_gpio_context dev)
{
    if (dev->bgpio != NULL) {
        BGpio_delete(dev->bgpio);
    }

    free(dev);
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_gpio_dir(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    int rc;

    if (dev->bgpio == NULL) {
        syslog(LOG_ERROR, "peripheralman: Invalid internal gpio handle");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    switch (dir) {
        case MRAA_GPIO_IN:
            rc = BGpio_setDirection(dev->bgpio, DIRECTION_IN);
            break;
        case MRAA_GPIO_OUT:
        case MRAA_GPIO_OUT_HIGH:
            rc = BGpio_setDirection(dev->bgpio, DIRECTION_OUT_INITIALLY_HIGH);
            break;
        case MRAA_GPIO_OUT_LOW:
            rc = BGpio_setDirection(dev->bgpio, DIRECTION_OUT_INITIALLY_LOW);
            break;
    }
    if (rc != 0) {
        syslog(LOG_ERROR, "peripheralman: Failed to switch direction");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_gpio_read_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t *dir)
{
    syslog(LOG_WARNING, "peripheralman: mraa_gpio_read_dir() dunction not implemented on this backend");
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_pman_gpio_write_replace(mraa_gpio_context dev, int val)
{
    int rc;

    if (dev->bgpio == NULL) {
        syslog(LOG_ERROR, "peripheralman: Invalid internal gpio handle");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = BGpio_setValue(dev->bgpio, val);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static int
mraa_pman_gpio_read_replace(mraa_gpio_context dev)
{
    int rc, val;

    if (dev->bgpio == NULL) {
        syslog(LOG_ERROR, "peripheralman: Invalid internal gpio handle");
        return -1;
    }

    rc = BGpio_getValue(dev->bgpio, &val);
    if (rc != 0) {
        syslog(LOG_ERROR, "peripheralman: Unable to read internal gpio")
        return -1;
    }

    return val;
}

static mraa_result_t
mraa_pman_gpio_edge_mode_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    int rc;

    if (dev->bgpio == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    switch (mode) {
        case MRAA_GPIO_EDGE_BOTH:
            rc = BGpio_setEdgeTriggerType(dev->bgpio, BOTH_EDGE);
            break;
        case MRAA_GPIO_EDGE_FALLING:
            rc = BGpio_setEdgeTriggerType(dev->bgpio, FALLING_EDGE);
            break;
        case MRAA_GPIO_EDGE_RISING:
            rc = BGpio_setEdgeTriggerType(dev->bgpio, RISING_EDGE);
            break;
        case MRAA_GPIO_EDGE_NONE:
            rc = BGpio_setEdgeTriggerType(dev->bgpio, NONE_EDGE);
            break;
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    return MRAA_SUCCESS;
};

static mraa_result_t
mraa_pman_gpio_isr_replace(mraa_gpio_context dev, mraa_gpio_edge_t edge, void (*fptr)(void*), void* args)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_pman_gpio_mode_replace(mraa_gpio_context dev, mraa_gpio_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

