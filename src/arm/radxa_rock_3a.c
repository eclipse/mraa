/*
 * Author: Nascs <nascs@radxa.com>
 * Copyright (c) Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "arm/radxa_rock_3a.h"
#include "common.h"

const char* radxa_rock_3a_serialdev[MRAA_RADXA_ROCK_3A_UART_COUNT] = { "/dev/ttyS0", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS5", "/dev/ttyS7", "/dev/ttyS8", "/dev/ttyS9" };

void
mraa_radxa_rock_3a_pininfo(mraa_board_t* board, int index, int gpio_chip, int gpio_line, mraa_pincapabilities_t pincapabilities_t, char* pin_name)
{

    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    strncpy(pininfo->name, pin_name, MRAA_PIN_NAME_SIZE);

    if(pincapabilities_t.gpio == 1) {
        pininfo->gpio.gpio_chip = gpio_chip;
        pininfo->gpio.gpio_line = gpio_line;
    }

    pininfo->capabilities = pincapabilities_t;

    pininfo->gpio.mux_total = 0;
}

mraa_board_t*
mraa_radxa_rock_3a()
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
    b->phy_pin_count = MRAA_RADXA_ROCK_3A_PIN_COUNT + 1;

    b->platform_name = PLATFORM_NAME_RADXA_ROCK_3A;
    b->chardev_capable = 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_ROCK_3A_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 0;
    b->uart_dev[1].index = 2;
    b->uart_dev[2].index = 3;
    b->uart_dev[3].index = 5;
    b->uart_dev[4].index = 7;
    b->uart_dev[5].index = 8;
    b->uart_dev[6].index = 9;
    b->uart_dev[0].device_path = (char*) radxa_rock_3a_serialdev[0];
    b->uart_dev[1].device_path = (char*) radxa_rock_3a_serialdev[1];
    b->uart_dev[2].device_path = (char*) radxa_rock_3a_serialdev[2];
    b->uart_dev[3].device_path = (char*) radxa_rock_3a_serialdev[3];
    b->uart_dev[4].device_path = (char*) radxa_rock_3a_serialdev[4];
    b->uart_dev[5].device_path = (char*) radxa_rock_3a_serialdev[5];
    b->uart_dev[6].device_path = (char*) radxa_rock_3a_serialdev[6];

    // I2C
    b->i2c_bus_count = MRAA_RADXA_ROCK_3A_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 1;
    b->i2c_bus[1].bus_id = 2;
    b->i2c_bus[2].bus_id = 3;

    // SPI
    b->spi_bus_count = MRAA_RADXA_ROCK_3A_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 3;

    // PWM
    b->pwm_dev_count = MRAA_RADXA_ROCK_3A_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[7].pwm.parent_id = 12;  // PWM12_M0
    b->pins[7].pwm.mux_total = 0;
    b->pins[11].pwm.parent_id = 14;  // PWM14_M0
    b->pins[11].pwm.mux_total = 0;
    b->pins[13].pwm.parent_id = 15; // PWM15_IR_M0
    b->pins[13].pwm.mux_total = 0;
    b->pins[15].pwm.parent_id = 1; // PWM1_M0
    b->pins[15].pwm.mux_total = 0;
    b->pins[19].pwm.parent_id = 15; // PWM15_IR_M1
    b->pins[19].pwm.mux_total = 0;
    b->pins[21].pwm.parent_id = 12; // PWM12_M1
    b->pins[21].pwm.mux_total = 0;
    b->pins[23].pwm.parent_id = 14; // PWM14_M1
    b->pins[23].pwm.mux_total = 0;
    b->pins[24].pwm.parent_id = 13; // PWM13_M1
    b->pins[24].pwm.mux_total = 0;
    b->pins[18].pwm.parent_id = 9; // PWM9_M0
    b->pins[18].pwm.mux_total = 0;
    b->pins[16].pwm.parent_id = 2; // PWM2_M1
    b->pins[16].pwm.mux_total = 0;
    b->pins[7].pwm.parent_id = 1; // PWM1_M1
    b->pins[7].pwm.mux_total = 0;

    // hardware V1.3/V1.31
    mraa_radxa_rock_3a_pininfo(b, 0, -1, -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_radxa_rock_3a_pininfo(b, 1, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_rock_3a_pininfo(b, 2, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_3a_pininfo(b, 3, 1, 0, (mraa_pincapabilities_t){1,1,0,0,0,1,0,1}, "GPIO1_A0");
    mraa_radxa_rock_3a_pininfo(b, 4, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_3a_pininfo(b, 5, 1, 1, (mraa_pincapabilities_t){1,1,0,0,0,1,0,1}, "GPIO1_A1");
    mraa_radxa_rock_3a_pininfo(b, 6, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 7, 0, 13, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO0_B5");
    mraa_radxa_rock_3a_pininfo(b, 8, 1, 24, (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, "GPIO0_D1"); // GPIO0_D1 was used by fiq_debugger, function GPIO cannot be enabled
    mraa_radxa_rock_3a_pininfo(b, 9, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 10, 1, 25, (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, "GPIO0_D1"); // GPIO0_D0 was used by fiq_debugger, function GPIO cannot be enabled
    mraa_radxa_rock_3a_pininfo(b, 11, 3, 20, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_C4");
    mraa_radxa_rock_3a_pininfo(b, 12, 3, 3, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_A3");
    mraa_radxa_rock_3a_pininfo(b, 13, 3, 21, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_C5");
    mraa_radxa_rock_3a_pininfo(b, 14, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 15, 0, 16, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO0_C0");
    mraa_radxa_rock_3a_pininfo(b, 16, 3, 1, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO0_B6");
    mraa_radxa_rock_3a_pininfo(b, 17, -1, -1, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "+3.3V");
    mraa_radxa_rock_3a_pininfo(b, 18, 3, 10, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_B2");
    mraa_radxa_rock_3a_pininfo(b, 19, 4, 19, (mraa_pincapabilities_t){1,1,1,0,1,0,0,0}, "GPIO4_C3");
    mraa_radxa_rock_3a_pininfo(b, 20, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 21, 4, 21, (mraa_pincapabilities_t){1,1,1,0,1,0,0,1}, "GPIO4_C5");
    mraa_radxa_rock_3a_pininfo(b, 22, 0, 17, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO0_C1");
    mraa_radxa_rock_3a_pininfo(b, 23, 4, 18, (mraa_pincapabilities_t){1,1,1,0,1,0,0,0}, "GPIO4_C2");
    mraa_radxa_rock_3a_pininfo(b, 24, 4, 22, (mraa_pincapabilities_t){1,1,1,0,1,0,0,1}, "GPIO4_C6");
    mraa_radxa_rock_3a_pininfo(b, 25, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 26, 4, 25, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_D1");
    mraa_radxa_rock_3a_pininfo(b, 27, 0, 14, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "GPIO0_B4"); // Hardware pull-up on this pin
    mraa_radxa_rock_3a_pininfo(b, 28, 0, 13, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "GPIO0_B3"); // Hardware pull-up on this pin
    mraa_radxa_rock_3a_pininfo(b, 29, 2, 31, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "GPIO2_D7");
    mraa_radxa_rock_3a_pininfo(b, 30, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 31, 3, 0, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO3_A0");
    mraa_radxa_rock_3a_pininfo(b, 32, 3, 18, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "GPIO3_C2");
    mraa_radxa_rock_3a_pininfo(b, 33, 3, 19, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO3_C3");
    mraa_radxa_rock_3a_pininfo(b, 34, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 35, 3, 4, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_A4");
    mraa_radxa_rock_3a_pininfo(b, 36, 3, 2, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_A2");
    mraa_radxa_rock_3a_pininfo(b, 37, 3, 16, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "SARADC_VIN5");
    mraa_radxa_rock_3a_pininfo(b, 38, 3, 6, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_A6");
    mraa_radxa_rock_3a_pininfo(b, 39, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_3a_pininfo(b, 40, 3, 5, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_A5");

    return b;
}
