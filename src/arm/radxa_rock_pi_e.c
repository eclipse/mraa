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
#include "arm/radxa_rock_pi_e.h"
#include "common.h"

const char* radxa_rock_pi_e_serialdev[MRAA_RADXA_ROCK_PI_E_UART_COUNT] = { "/dev/ttyS1", "/dev/ttyS2"};

void
mraa_radxa_rock_pi_e_pininfo(mraa_board_t* board, int index, int gpio_chip, int gpio_line, mraa_pincapabilities_t pincapabilities_t, char* pin_name)
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
mraa_radxa_rock_pi_e()
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
    b->phy_pin_count = MRAA_RADXA_ROCK_PI_E_PIN_COUNT + 1;

    b->platform_name = PLATFORM_NAME_RADXA_ROCK_PI_E;
    b->chardev_capable = 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_ROCK_PI_E_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 1;
    b->uart_dev[1].index = 2;
    b->uart_dev[0].device_path = (char*) radxa_rock_pi_e_serialdev[0];
    b->uart_dev[1].device_path = (char*) radxa_rock_pi_e_serialdev[1];

    // I2C
    b->i2c_bus_count = MRAA_RADXA_ROCK_PI_E_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 1;

    // SPI
    b->spi_bus_count = MRAA_RADXA_ROCK_PI_E_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 3;

    // PWM
    b->pwm_dev_count = MRAA_RADXA_ROCK_PI_E_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[33].pwm.parent_id = 2; // pwm2
    b->pins[33].pwm.mux_total = 0;

    // hardware V3.0
    mraa_radxa_rock_pi_e_pininfo(b, 0, -1, -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_radxa_rock_pi_e_pininfo(b, 1, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_rock_pi_e_pininfo(b, 2, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_pi_e_pininfo(b, 3, 3, 4, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "GPIO3_A4");
    mraa_radxa_rock_pi_e_pininfo(b, 4, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_pi_e_pininfo(b, 5, 3, 6, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "GPIO3_A6");
    mraa_radxa_rock_pi_e_pininfo(b, 6, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 7, 1, 28, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO1_D4");
    mraa_radxa_rock_pi_e_pininfo(b, 8, 2, 0, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "GPIO2_A0");
    mraa_radxa_rock_pi_e_pininfo(b, 9, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 10, 2, 1, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "GPIO2_A1");
    mraa_radxa_rock_pi_e_pininfo(b, 11, 2, 2, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_A2");
    mraa_radxa_rock_pi_e_pininfo(b, 12, 2, 18, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C2");
    mraa_radxa_rock_pi_e_pininfo(b, 13, 2, 3, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GPIO2_A3"); // Hardware cannot output low level
    mraa_radxa_rock_pi_e_pininfo(b, 14, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 15, 0, 27, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO0_D3");
    mraa_radxa_rock_pi_e_pininfo(b, 16, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "USB20DM");
    mraa_radxa_rock_pi_e_pininfo(b, 17, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_rock_pi_e_pininfo(b, 18, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "USB20DP");
    mraa_radxa_rock_pi_e_pininfo(b, 19, 3, 1, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO3_A1");
    mraa_radxa_rock_pi_e_pininfo(b, 20, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 21, 3, 2,  (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO3_A2");
    mraa_radxa_rock_pi_e_pininfo(b, 22, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, "SARADC_IN1");
    mraa_radxa_rock_pi_e_pininfo(b, 23, 3, 0,  (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO3_A0");
    mraa_radxa_rock_pi_e_pininfo(b, 24, 3, 8,  (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO3_B0");
    mraa_radxa_rock_pi_e_pininfo(b, 25, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 26, 2, 12, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B4");
    mraa_radxa_rock_pi_e_pininfo(b, 27, 2, 4,  (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO2_A4");
    mraa_radxa_rock_pi_e_pininfo(b, 28, 2, 5,  (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO2_A5");
    mraa_radxa_rock_pi_e_pininfo(b, 29, 2, 20, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C4");
    mraa_radxa_rock_pi_e_pininfo(b, 30, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 31, 2, 21, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C5");
    mraa_radxa_rock_pi_e_pininfo(b, 32, 2, 16, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C0");
    mraa_radxa_rock_pi_e_pininfo(b, 33, 2, 6, (mraa_pincapabilities_t){1,0,1,0,0,0,0,0}, "GPIO2_A6"); // tied to an IRQ
    mraa_radxa_rock_pi_e_pininfo(b, 34, -1, -1,(mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 35, 2, 17, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C1");
    mraa_radxa_rock_pi_e_pininfo(b, 36, 2, 15, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B7");
    mraa_radxa_rock_pi_e_pininfo(b, 37, 2, 22, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C6");
    mraa_radxa_rock_pi_e_pininfo(b, 38, 2, 19, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C3");
    mraa_radxa_rock_pi_e_pininfo(b, 39, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_pi_e_pininfo(b, 40, 2, 23, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C7");

    return b;
}
