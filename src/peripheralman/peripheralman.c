/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
#include <string.h>
#include <syslog.h>

#include "mraa_internal.h"
#include "peripheralman.h"

BPeripheralManagerClient *client = NULL;
char **gpios = NULL;
int gpios_count = 0;
char **i2c_busses = NULL;
int i2c_busses_count = 0;
char **spi_busses = NULL;
int spi_busses_count = 0;
char **uart_devices = NULL;
int uart_busses_count = 0;

static mraa_result_t
mraa_pman_gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    int rc = BPeripheralManagerClient_openGpio(client, gpios[pin], &dev->bgpio);
    if (rc != 0) {
        syslog(LOG_ERR, "peripheralmanager: Failed to init gpio");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    dev->pin = pin;
    dev->phy_pin = pin;

    return MRAA_SUCCESS;
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
mraa_pman_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    int rc;

    if (dev->bgpio == NULL) {
        syslog(LOG_ERR, "peripheralman: Invalid internal gpio handle");
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
        syslog(LOG_ERR, "peripheralman: Failed to switch direction");
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
        syslog(LOG_ERR, "peripheralman: Invalid internal gpio handle");
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
        syslog(LOG_ERR, "peripheralman: Invalid internal gpio handle");
        return -1;
    }

    rc = BGpio_getValue(dev->bgpio, &val);
    if (rc != 0) {
        syslog(LOG_ERR, "peripheralman: Unable to read internal gpio");
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

mraa_board_t*
mraa_peripheralman_plat_init()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    if (client != NULL) {
        BPeripheralManagerClient_delete(client);
    }

    client = BPeripheralManagerClient_new();
    if (client == NULL) {
        return NULL;
    }

    // error checking?
    gpios = BPeripheralManagerClient_listGpio(client, &gpios_count);
    i2c_busses = BPeripheralManagerClient_listI2cBuses(client, &i2c_busses_count);
    spi_busses = BPeripheralManagerClient_listSpiBuses(client, &spi_busses_count);
    uart_devices = BPeripheralManagerClient_listUartDevices(client, &uart_busses_count);

    b->platform_name = "peripheralmanager";
    // query this from peripheral manager?
    b->platform_version = "1.0";

    // disable AIO support
    b->aio_count = 0;
    b->adc_supported = 0;

    b->gpio_count = gpios_count;
    b->phy_pin_count = gpios_count;
    b->i2c_bus_count = i2c_busses_count;
    b->spi_bus_count = spi_busses_count;
    b->uart_dev_count = uart_busses_count;
    b->def_i2c_bus = 0;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b);
        return NULL;
    }

    int i = 0;
    for (; i < gpios_count; i++) {
        b->pins[i].name = gpios[i];
        b->pins[i].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[i].gpio.pinmap = -1;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        free(b);
        return NULL;
    }

    b->adv_func->gpio_init_internal_replace = &mraa_pman_gpio_init_internal_replace;
    b->adv_func->gpio_close_replace = &mraa_pman_gpio_close_replace;
    b->adv_func->gpio_dir_replace = &mraa_pman_gpio_dir_replace;
    b->adv_func->gpio_read_dir_replace = &mraa_pman_gpio_read_dir_replace;
    b->adv_func->gpio_write_replace = &mraa_pman_gpio_write_replace;
    b->adv_func->gpio_read_replace = &mraa_pman_gpio_read_replace;
    b->adv_func->gpio_edge_mode_replace = &mraa_pman_gpio_edge_mode_replace;
    b->adv_func->gpio_mode_replace = &mraa_pman_gpio_mode_replace;
    b->adv_func->gpio_isr_replace = &mraa_pman_gpio_isr_replace;

#if 0
    b->adv_func->aio_init_internal_replace = &mraa_pman_aio_init_internal_replace;
    b->adv_func->aio_read_replace = &mraa_pman_aio_read;

    b->adv_func->pwm_init_internal_replace = &mraa_pman_pwm_init_internal_replace;
    b->adv_func->pwm_write_replace = &mraa_pman_pwm_write_replace;
    b->adv_func->pwm_read_replace = &mraa_pman_pwm_read_replace;
    b->adv_func->pwm_enable_replace = &mraa_pman_pwm_enable_replace;

    b->adv_func->i2c_init_bus_replace = &mraa_pman_i2c_init_bus_replace;
    b->adv_func->i2c_set_frequency_replace = &mraa_pman_i2c_frequency;
    b->adv_func->i2c_address_replace = &mraa_pman_i2c_address;
    b->adv_func->i2c_read_replace = &mraa_pman_i2c_read;
    b->adv_func->i2c_read_byte_replace = &mraa_pman_i2c_read_byte;
    b->adv_func->i2c_read_byte_data_replace = &mraa_pman_i2c_read_byte_data;
    b->adv_func->i2c_read_word_data_replace = &mraa_pman_i2c_read_word_data;
    b->adv_func->i2c_read_bytes_data_replace = &mraa_pman_i2c_read_bytes_data;
    b->adv_func->i2c_write_replace = &mraa_pman_i2c_write;
    b->adv_func->i2c_write_byte_replace = &mraa_pman_i2c_write_byte;
    b->adv_func->i2c_write_byte_data_replace = &mraa_pman_i2c_write_byte_data;
    b->adv_func->i2c_write_word_data_replace = &mraa_pman_i2c_write_word_data;
    b->adv_func->i2c_stop_replace = &mraa_pman_i2c_stop;
#endif
    return b;
}

mraa_platform_t
mraa_peripheralman_platform()
{
    plat = mraa_peripheralman_plat_init();

    return MRAA_ANDROID_PERIPHERALMANAGER;
}

static void
free_resources(char ***resources, int count)
{
    int i;

    if (*resources != NULL) {
        for(i = 0; i < count; i++) {
            free((*resources)[i]);
        }
        free(*resources);
    }

    *resources = NULL;
}

void
pman_mraa_deinit()
{
    free_resources(&uart_devices, uart_busses_count);
    free_resources(&spi_busses, spi_busses_count);
    free_resources(&i2c_busses, i2c_busses_count);
    free_resources(&gpios, gpios_count);

    if (client != NULL) {
        BPeripheralManagerClient_delete(client);
        client = NULL;
    }
}
