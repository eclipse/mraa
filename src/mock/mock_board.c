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

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "mock/mock_board.h"

#define PLATFORM_NAME "MRAA mock platform"

mraa_result_t
mraa_mock_gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    dev->value_fp = -1;
    dev->isr_value_fp = -1;
    dev->isr_thread_terminating = 0;
    dev->phy_pin = pin;
    // We are always the owner
    dev->owner = 1;
#ifndef HAVE_PTHREAD_CANCEL
    dev->isr_control_pipe[0] = dev->isr_control_pipe[1] = -1;
#endif

    // We start as INPUT and LOW
    dev->mock_dir = MRAA_GPIO_IN;
    dev->mock_state = 0;

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_gpio_close_replace(mraa_gpio_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    switch (dir) {
        case MRAA_GPIO_OUT_HIGH:
            dev->mock_dir = MRAA_GPIO_OUT;
            return mraa_gpio_write(dev, 1);
        case MRAA_GPIO_OUT_LOW:
            dev->mock_dir = MRAA_GPIO_OUT;
            return mraa_gpio_write(dev, 0);
        case MRAA_GPIO_IN:
        case MRAA_GPIO_OUT:
            dev->mock_dir = dir;
            return MRAA_SUCCESS;
        default:
            syslog(LOG_ERR, "gpio: dir: invalid direction '%d' to set", (int) dir);
            return MRAA_ERROR_INVALID_PARAMETER;
    }
}

mraa_result_t
mraa_mock_gpio_read_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t* dir)
{
    *dir = dev->mock_dir;
    return MRAA_SUCCESS;
}

int
mraa_mock_gpio_read_replace(mraa_gpio_context dev)
{
    return dev->mock_state;
}

mraa_result_t
mraa_mock_gpio_write_replace(mraa_gpio_context dev, int value)
{
    if ((value < 0) || (value > 1)) {
        syslog(LOG_ERR, "gpio: write: incorrect value '%d' passed to write(), must be 0 or 1", value);
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    if (dev->mock_dir == MRAA_GPIO_IN) {
        syslog(LOG_ERR, "gpio: write: cannot write to pin set to INPUT");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->mock_state = value;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_gpio_edge_mode_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

mraa_result_t
mraa_mock_gpio_isr_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode, void (*fptr)(void*), void* args)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

mraa_result_t
mraa_mock_gpio_isr_exit_replace(mraa_gpio_context dev)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

mraa_result_t
mraa_mock_gpio_mode_replace(mraa_gpio_context dev, mraa_gpio_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

mraa_result_t
mraa_mock_aio_init_internal_replace(mraa_aio_context dev, int pin)
{
    dev->channel = pin;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_aio_close_replace(mraa_aio_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}

int
mraa_mock_aio_read_replace(mraa_aio_context dev)
{
    // return some random number between 0 and max value, based on the resolution
    int max_value = (1 << dev->value_bit) - 1;
    srand(time(NULL));
    return rand() % max_value;
}

mraa_board_t*
mraa_mock_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    // General board definitions
    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = MRAA_MOCK_PINCOUNT;
    b->gpio_count = 1;
    b->aio_count = 1;
    b->adc_raw = 12;
    b->adc_supported = 10;
    b->i2c_bus_count = 0;
    b->spi_bus_count = 0;

    b->pwm_default_period = 0;
    b->pwm_max_period = 0;
    b->pwm_min_period = 0;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_MOCK_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    // Replace functions
    b->adv_func->gpio_init_internal_replace = &mraa_mock_gpio_init_internal_replace;
    b->adv_func->gpio_close_replace = &mraa_mock_gpio_close_replace;
    b->adv_func->gpio_dir_replace = &mraa_mock_gpio_dir_replace;
    b->adv_func->gpio_read_dir_replace = &mraa_mock_gpio_read_dir_replace;
    b->adv_func->gpio_read_replace = &mraa_mock_gpio_read_replace;
    b->adv_func->gpio_write_replace = &mraa_mock_gpio_write_replace;
    b->adv_func->gpio_edge_mode_replace = &mraa_mock_gpio_edge_mode_replace;
    b->adv_func->gpio_isr_replace = &mraa_mock_gpio_isr_replace;
    b->adv_func->gpio_isr_exit_replace = &mraa_mock_gpio_isr_exit_replace;
    b->adv_func->gpio_mode_replace = &mraa_mock_gpio_mode_replace;
    b->adv_func->aio_init_internal_replace = &mraa_mock_aio_init_internal_replace;
    b->adv_func->aio_close_replace = &mraa_mock_aio_close_replace;
    b->adv_func->aio_read_replace = &mraa_mock_aio_read_replace;

    // Pin definitions
    int pos = 0;

    strncpy(b->pins[pos].name, "GPIO0", 8);
    b->pins[pos].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 0;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "ADC0", 8);
    b->pins[pos].capabilites = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[pos].aio.pinmap = 0;
    b->pins[pos].aio.mux_total = 0;
    pos++;

    return b;

error:
    syslog(LOG_CRIT, "MRAA mock: Platform failed to initialise");
    free(b);
    return NULL;
}
