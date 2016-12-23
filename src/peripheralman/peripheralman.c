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

    int gpios_count, i2c_busses_count, spi_busses_count, uart_busses_count;
    int gpios = BPeripheralManagerClient_listGpio(client, &gpios_count);
    int i2c_busses = BPeripheralManagerClient_listI2cBuses(client, &i2c_busses_count);
    int spi_busses = BPeripheralManagerClient_listSpiBuses(client, &spi_busses_count);
    int uart_devices = BPeripheralManagerClient_listUartDevices(client, &uart_busses_count);

    b->platform_name = "peripheralmanager";
    // query this from peripheral manager?
    b->platform_version = "1.0";
    b->gpio_count = 14;
    // disable AIO support
    b->aio_count = 0;
    b->adc_supported = 0;

    // get this from PM
    b->phy_pin_count = 20;
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->pwm_min_period = 2048;
    b->pwm_max_period = 2048;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b);
        return NULL;
    }

#if 0
    strncpy(b->pins[0].name, "IO0", 8);
    b->pins[0].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[0].gpio.pinmap = 0;
    strncpy(b->pins[1].name, "IO1", 8);
    b->pins[1].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[1].gpio.pinmap = 1;
    strncpy(b->pins[2].name, "IO2", 8);
    b->pins[2].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[2].gpio.pinmap = 2;
    strncpy(b->pins[3].name, "IO3", 8);
    b->pins[3].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[3].gpio.pinmap = 3;
    strncpy(b->pins[4].name, "IO4", 8);
    b->pins[4].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[4].gpio.pinmap = 4;
    strncpy(b->pins[5].name, "IO5", 8);
    b->pins[5].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[5].gpio.pinmap = 5;
    strncpy(b->pins[6].name, "IO6", 8);
    b->pins[6].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[6].gpio.pinmap = 6;
    strncpy(b->pins[7].name, "IO7", 8);
    b->pins[7].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[7].gpio.pinmap = 7;
    strncpy(b->pins[8].name, "IO8", 8);
    b->pins[8].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[8].gpio.pinmap = 8;
    strncpy(b->pins[9].name, "IO9", 8);
    b->pins[9].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[9].gpio.pinmap = 9;
    strncpy(b->pins[10].name, "IO10", 8);
    b->pins[10].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[10].gpio.pinmap = 10;
    strncpy(b->pins[11].name, "IO11", 8);
    b->pins[11].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[11].gpio.pinmap = 11;
    strncpy(b->pins[12].name, "IO12", 8);
    b->pins[12].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[12].gpio.pinmap = 12;
    strncpy(b->pins[13].name, "IO13", 8);
    b->pins[13].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[13].gpio.pinmap = 13;
    strncpy(b->pins[10].name, "A0", 8);
    b->pins[14].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[14].gpio.pinmap = 14;
    b->pins[14].aio.pinmap = 14;
    strncpy(b->pins[11].name, "A1", 8);
    b->pins[15].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[15].gpio.pinmap = 15;
    b->pins[15].aio.pinmap = 15;
    strncpy(b->pins[12].name, "A2", 8);
    b->pins[16].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[16].gpio.pinmap = 16;
    b->pins[16].aio.pinmap = 16;
    strncpy(b->pins[13].name, "A3", 8);
    b->pins[17].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[17].gpio.pinmap = 17;
    b->pins[17].aio.pinmap = 17;
    strncpy(b->pins[13].name, "A4", 8);
    b->pins[18].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[18].gpio.pinmap = 18;
    b->pins[18].aio.pinmap = 18;
    strncpy(b->pins[13].name, "A5", 8);
    b->pins[19].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[19].gpio.pinmap = 19;
    b->pins[19].aio.pinmap = 19;
#endif

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        free(b);
        return NULL;
    }

#if 0
    b->adv_func->gpio_init_internal_replace = &mraa_firmata_gpio_init_internal_replace;
    b->adv_func->gpio_mode_replace = &mraa_firmata_gpio_mode_replace;
    b->adv_func->gpio_dir_replace = &mraa_firmata_gpio_dir_replace;
    b->adv_func->gpio_edge_mode_replace = &mraa_firmata_gpio_edge_mode_replace;
    b->adv_func->gpio_interrupt_handler_init_replace = &mraa_firmata_gpio_interrupt_handler_init_replace;
    b->adv_func->gpio_wait_interrupt_replace = &mraa_firmata_gpio_wait_interrupt_replace;
    b->adv_func->gpio_read_replace = &mraa_firmata_gpio_read_replace;
    b->adv_func->gpio_write_replace = &mraa_firmata_gpio_write_replace;
    b->adv_func->gpio_close_replace = &mraa_firmata_gpio_close_replace;

    b->adv_func->aio_init_internal_replace = &mraa_firmata_aio_init_internal_replace;
    b->adv_func->aio_read_replace = &mraa_firmata_aio_read;

    b->adv_func->pwm_init_internal_replace = &mraa_firmata_pwm_init_internal_replace;
    b->adv_func->pwm_write_replace = &mraa_firmata_pwm_write_replace;
    b->adv_func->pwm_read_replace = &mraa_firmata_pwm_read_replace;
    b->adv_func->pwm_enable_replace = &mraa_firmata_pwm_enable_replace;

    b->adv_func->i2c_init_bus_replace = &mraa_firmata_i2c_init_bus_replace;
    b->adv_func->i2c_set_frequency_replace = &mraa_firmata_i2c_frequency;
    b->adv_func->i2c_address_replace = &mraa_firmata_i2c_address;
    b->adv_func->i2c_read_replace = &mraa_firmata_i2c_read;
    b->adv_func->i2c_read_byte_replace = &mraa_firmata_i2c_read_byte;
    b->adv_func->i2c_read_byte_data_replace = &mraa_firmata_i2c_read_byte_data;
    b->adv_func->i2c_read_word_data_replace = &mraa_firmata_i2c_read_word_data;
    b->adv_func->i2c_read_bytes_data_replace = &mraa_firmata_i2c_read_bytes_data;
    b->adv_func->i2c_write_replace = &mraa_firmata_i2c_write;
    b->adv_func->i2c_write_byte_replace = &mraa_firmata_i2c_write_byte;
    b->adv_func->i2c_write_byte_data_replace = &mraa_firmata_i2c_write_byte_data;
    b->adv_func->i2c_write_word_data_replace = &mraa_firmata_i2c_write_word_data;
    b->adv_func->i2c_stop_replace = &mraa_firmata_i2c_stop;
#endif
    return b;
}

mraa_platform_t
mraa_peripheralman_platform()
{
    plat = mraa_peripheralman_plat_init();

    return MRAA_ANDROID_PERIPHERALMANAGER;
}
