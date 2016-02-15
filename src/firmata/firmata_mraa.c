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

#include "mraa_internal.h"
#include "firmata/firmata_mraa.h"
#include "firmata/firmata.h"

static t_firmata *firmata_dev;

static mraa_result_t
mraa_firmata_gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    dev->pin = pin;
    dev->phy_pin = pin;

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_firmata_gpio_mode_replace(mraa_gpio_context dev, mraa_gpio_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static int
mraa_firmata_gpio_read_replace(mraa_gpio_context dev)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_firmata_gpio_write_replace(mraa_gpio_context dev, int write_value)
{
    if (write_value == 0) {
        firmata_digitalWrite(firmata_dev, dev->phy_pin, LOW);
    }
    else {
        firmata_digitalWrite(firmata_dev, dev->phy_pin, HIGH);
    }
}

static mraa_result_t
mraa_firmata_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    switch (dir) {
        case MRAA_GPIO_IN:
	    firmata_pinMode(firmata_dev, dev->phy_pin, MODE_OUTPUT);
	case MRAA_GPIO_OUT:
	    firmata_pinMode(firmata_dev, dev->phy_pin, MODE_INPUT);
        default:
            return MRAA_ERROR_INVALID_PARAMETER;
    }

    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_firmata_init(const char* uart_dev)
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->platform_name = "firmata";
    b->platform_version = "2.3";
    b->gpio_count = 20;
    b->phy_pin_count = b->gpio_count;

    b->pins = (mraa_pininfo_t*) calloc(b->gpio_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b);
        return NULL;
    }   

    strncpy(b->pins[13].name, "IO13", 8);
    b->pins[13].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[13].gpio.pinmap = 13;

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        free(b);
        return NULL;
    }

    b->adv_func->gpio_init_internal_replace = &mraa_firmata_gpio_init_internal_replace;
    b->adv_func->gpio_mode_replace = &mraa_firmata_gpio_mode_replace;
    b->adv_func->gpio_dir_replace = &mraa_firmata_gpio_dir_replace;
    b->adv_func->gpio_read_replace = &mraa_firmata_gpio_read_replace;
    b->adv_func->gpio_write_replace = &mraa_firmata_gpio_write_replace;

    firmata_dev = firmata_new(uart_dev);
#if 0
    while (!firmata_dev->isReady) {
       firmata_pull(firmata_dev);
    }
#endif

    return b;
}

mraa_platform_t
mraa_firmata_platform(mraa_board_t* board, const char* uart_dev)
{
   /** 
    * Firmata boards are not something we can detect so we just trust the user
    * to initialise them themselves and is the only platform type not to be
    * initialised from mraa_init(). Good luck!
    */
    mraa_board_t* sub_plat = NULL;

    sub_plat = mraa_firmata_init(uart_dev);
    if (sub_plat != NULL) {
        sub_plat->platform_type = MRAA_GENERIC_FIRMATA;
        board->sub_platform = sub_plat;
        return sub_plat->platform_type;
    }

    return MRAA_NULL_PLATFORM;
}

