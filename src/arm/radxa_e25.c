/*
 * Author: Nascs <nascs@radxa.com>
 * Copyright (c) 2023 Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "arm/radxa_e25.h"
#include "common.h"

const char* radxa_e25_serialdev[MRAA_RADXA_E25_UART_COUNT] = { "/dev/ttyS3", "/dev/ttyS5", "/dev/ttyS7" };

void
mraa_radxa_e25_pininfo(mraa_board_t* board, int index, int gpio_chip, int gpio_line, mraa_pincapabilities_t pincapabilities_t, char* pin_name)
{

    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    strncpy(pininfo->name, pin_name, MRAA_PIN_NAME_SIZE);

    if (pincapabilities_t.gpio == 1) {
        pininfo->gpio.gpio_chip = gpio_chip;
        pininfo->gpio.gpio_line = gpio_line;
    }

    pininfo->capabilities = pincapabilities_t;

    pininfo->gpio.mux_total = 0;
}

mraa_board_t*
mraa_radxa_e25()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b);
        return NULL;
    }

    // pin mux for buses are setup by default by kernel so tell mraa to ignore them
    b->no_bus_mux = 1;
    b->phy_pin_count = MRAA_RADXA_E25_PIN_COUNT + 1;

    if (mraa_file_contains("/proc/device-tree/model", PLATFORM_NAME_RADXA_E25)) {
        b->platform_name = PLATFORM_NAME_RADXA_E25;
    } else {
        printf("An unknown product detected. Fail early...\n");
        exit(-1);
    }

    b->chardev_capable = 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_E25_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 3;
    b->uart_dev[1].index = 5;
    b->uart_dev[2].index = 7;
    b->uart_dev[0].device_path = (char*) radxa_e25_serialdev[0];
    b->uart_dev[1].device_path = (char*) radxa_e25_serialdev[1];
    b->uart_dev[2].device_path = (char*) radxa_e25_serialdev[2];

    // I2C
    b->i2c_bus_count = MRAA_RADXA_E25_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 3;

    // SPI
    b->spi_bus_count = MRAA_RADXA_E25_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[1].bus_id = 1;

    // PWM
    b->pwm_dev_count = MRAA_RADXA_E25_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[7].pwm.parent_id = 12;  // pwm12-m0
    b->pins[7].pwm.mux_total = 0;
    b->pins[11].pwm.parent_id = 14;  // pwm14-m0
    b->pins[11].pwm.mux_total = 0;
    b->pins[13].pwm.parent_id = 1; // pwm15-m0
    b->pins[13].pwm.mux_total = 0;
    b->pins[18].pwm.parent_id = 7; // pwm7-m0
    b->pins[18].pwm.mux_total = 0;
    b->pins[24].pwm.parent_id = 13; // pwm13-m1
    b->pins[24].pwm.mux_total = 0;
    b->pins[26].pwm.parent_id = 4;  // pwm13-m0
    b->pins[26].pwm.mux_total = 0;

    mraa_radxa_e25_pininfo(b, 0, -1, -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_radxa_e25_pininfo(b, 1, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3.3V");
    mraa_radxa_e25_pininfo(b, 2, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5.0V");
    mraa_radxa_e25_pininfo(b, 3, 1, 0, (mraa_pincapabilities_t){1,1,0,0,0,1,0,1}, "GPIO1_A0");
    mraa_radxa_e25_pininfo(b, 4, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5.0V");
    mraa_radxa_e25_pininfo(b, 5, 1, 1, (mraa_pincapabilities_t){1,1,0,0,0,1,0,1}, "GPIO1_A1");
    mraa_radxa_e25_pininfo(b, 6, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_e25_pininfo(b, 7, 3, 15, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_B7");
    mraa_radxa_e25_pininfo(b, 8, 3, 18, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO3_C2");
    mraa_radxa_e25_pininfo(b, 9, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_e25_pininfo(b, 10, 3, 19, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO3_C3");
    mraa_radxa_e25_pininfo(b, 11, 3, 20, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_C4");
    mraa_radxa_e25_pininfo(b, 12, 3, 3, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_A3");
    mraa_radxa_e25_pininfo(b, 13, 3, 21, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_C5");
    mraa_radxa_e25_pininfo(b, 14, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_e25_pininfo(b, 15, 3, 17, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO3_C1");
    mraa_radxa_e25_pininfo(b, 16, 2, 26, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO2_D2");
    mraa_radxa_e25_pininfo(b, 17, 3, 1, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO3_A1");
    mraa_radxa_e25_pininfo(b, 18, 0, 22, (mraa_pincapabilities_t){1,1,1,0,1,0,0,0}, "GPIO0_C6");
    mraa_radxa_e25_pininfo(b, 19, 2, 25, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO2_D1");
    mraa_radxa_e25_pininfo(b, 20, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_e25_pininfo(b, 21, 2, 24, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO2_D0");
    mraa_radxa_e25_pininfo(b, 22, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "SARADC_VIN5");
    mraa_radxa_e25_pininfo(b, 23, 2, 27, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO2_D3");
    mraa_radxa_e25_pininfo(b, 24, 4, 22, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO4_C6");
    mraa_radxa_e25_pininfo(b, 25, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_e25_pininfo(b, 26, 3, 16, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_C0");

    return b;
}
