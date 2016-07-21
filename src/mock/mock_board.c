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
#include "mock/mock_board_gpio.h"
#include "mock/mock_board_aio.h"

#define PLATFORM_NAME "MRAA mock platform"

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
