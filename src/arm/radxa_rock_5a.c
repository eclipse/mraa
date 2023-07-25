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

#include "arm/radxa_rock_5a.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"

const char* radxa_rock_5a_serialdev[MRAA_RADXA_ROCK_5A_UART_COUNT] = { "/dev/ttyS2", "/dev/ttyS4", "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8"};

void
mraa_radxa_rock_5a_pininfo(mraa_board_t* board, int index, int gpio_chip, int gpio_line, mraa_pincapabilities_t pincapabilities_t, char* pin_name)
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
mraa_radxa_rock_5a()
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
    b->phy_pin_count = MRAA_RADXA_ROCK_5A_PIN_COUNT + 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_ROCK_5A_UART_COUNT;
    b->platform_name = PLATFORM_NAME_RADXA_ROCK_5A;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 2;
    b->uart_dev[1].index = 4;
    b->uart_dev[2].index = 6;
    b->uart_dev[3].index = 7;
    b->uart_dev[4].index = 8;
    b->uart_dev[0].device_path = (char*) radxa_rock_5a_serialdev[0];
    b->uart_dev[1].device_path = (char*) radxa_rock_5a_serialdev[1];
    b->uart_dev[2].device_path = (char*) radxa_rock_5a_serialdev[2];
    b->uart_dev[3].device_path = (char*) radxa_rock_5a_serialdev[3];
    b->uart_dev[4].device_path = (char*) radxa_rock_5a_serialdev[4];

    // I2C
    b->i2c_bus_count = MRAA_RADXA_ROCK_5A_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 1;
    b->i2c_bus[1].bus_id = 2;
    b->i2c_bus[2].bus_id = 4;
    b->i2c_bus[3].bus_id = 6;
    b->i2c_bus[4].bus_id = 8;

    // SPI
    b->spi_bus_count = MRAA_RADXA_ROCK_5A_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[1].bus_id = 4;

    // PWM
    b->pwm_dev_count = MRAA_RADXA_ROCK_5A_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[3].pwm.parent_id = 15;    // PWM15_M3
    b->pins[3].pwm.mux_total = 0;
    b->pins[3].pwm.pinmap = 0;
    b->pins[5].pwm.parent_id = 14;    // PWM14_M2
    b->pins[5].pwm.mux_total = 0;
    b->pins[5].pwm.pinmap = 0;
    b->pins[11].pwm.parent_id = 15;   // PWM15_M1
    b->pins[11].pwm.mux_total = 0;
    b->pins[11].pwm.pinmap = 0;
    b->pins[13].pwm.parent_id = 14;   // PWM14_M1
    b->pins[13].pwm.mux_total = 0;
    b->pins[13].pwm.pinmap = 0;
    b->pins[15].pwm.parent_id = 11;  // PWM11_M1
    b->pins[15].pwm.mux_total = 0;
    b->pins[15].pwm.pinmap = 0;
    b->pins[23].pwm.parent_id = 0;   // PWM0_M2
    b->pins[23].pwm.mux_total = 0;
    b->pins[23].pwm.pinmap = 0;
    b->pins[27].pwm.parent_id = 6;   // PWM6_M0
    b->pins[27].pwm.mux_total = 0;
    b->pins[27].pwm.pinmap = 0;
    b->pins[28].pwm.parent_id = 7;   // PWM7_M0
    b->pins[28].pwm.mux_total = 0;
    b->pins[28].pwm.pinmap = 0;
    b->pins[28].pwm.parent_id = 1;   // PWM1_M2
    b->pins[28].pwm.mux_total = 0;
    b->pins[28].pwm.pinmap = 0;

    // AIO
    b->aio_count = MRAA_RADXA_ROCK_5A_AIO_COUNT;
    b->adc_raw = 10;
    b->adc_supported = 10;
    b->aio_dev[0].pin = 37;
    b->aio_non_seq = 1;
    b->chardev_capable = 1;

    // Hardware X1.2
    mraa_radxa_rock_5a_pininfo(b, 0,  -1, -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_radxa_rock_5a_pininfo(b, 1,  -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_rock_5a_pininfo(b, 2,  -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_5a_pininfo(b, 3,   1, 31, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO1_D7");
    mraa_radxa_rock_5a_pininfo(b, 4,  -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_5a_pininfo(b, 5,   1, 30, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO1_D6");
    mraa_radxa_rock_5a_pininfo(b, 6,  -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 7,   1, 11, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B3");
    mraa_radxa_rock_5a_pininfo(b, 8,   0, 13, (mraa_pincapabilities_t){1,0,0,0,0,1,0,1}, "GPIO0_B5");  // Used by fiq_debugger
    mraa_radxa_rock_5a_pininfo(b, 9,  -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 10,  0, 14, (mraa_pincapabilities_t){1,0,0,0,0,1,0,1}, "GPIO0_B6");  // Used by fiq_debugger
    mraa_radxa_rock_5a_pininfo(b, 11,  4, 11, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO4_B3");  // I2C-M0 is used by on-board devices
    mraa_radxa_rock_5a_pininfo(b, 12,  4,  1, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A1");
    mraa_radxa_rock_5a_pininfo(b, 13,  4, 10, (mraa_pincapabilities_t){1,1,1,0,1,0,0,0}, "GPIO4_B2");  // I2C-M0 is used by on-board devices
    mraa_radxa_rock_5a_pininfo(b, 14, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 15,  4, 12, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO4_B4");
    mraa_radxa_rock_5a_pininfo(b, 16,  1,  5, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO1_A5");
    mraa_radxa_rock_5a_pininfo(b, 17, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_rock_5a_pininfo(b, 18,  1,  8, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO1_B0");
    mraa_radxa_rock_5a_pininfo(b, 19,  1,  1, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "GPIO1_A1");
    mraa_radxa_rock_5a_pininfo(b, 20, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 21,  1,  0, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "GPIO1_A0");
    mraa_radxa_rock_5a_pininfo(b, 22,  1, 13, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B5");
    mraa_radxa_rock_5a_pininfo(b, 23,  1,  2, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO1_A2");
    mraa_radxa_rock_5a_pininfo(b, 24,  1,  3, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO1_A3");
    mraa_radxa_rock_5a_pininfo(b, 25, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 26,  1,  4, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO1_A4");
    mraa_radxa_rock_5a_pininfo(b, 27,  0, 23, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO0_C7");
    mraa_radxa_rock_5a_pininfo(b, 28,  0, 24, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO0_D0");
    mraa_radxa_rock_5a_pininfo(b, 29,  1, 10, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B2");
    mraa_radxa_rock_5a_pininfo(b, 30, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 31,  1,  9, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO1_B1");
    mraa_radxa_rock_5a_pininfo(b, 32,  4,  8, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "GPIO4_B0");
    mraa_radxa_rock_5a_pininfo(b, 33,  1, 12, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B4");
    mraa_radxa_rock_5a_pininfo(b, 34, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 35,  4,  0, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A0");
    mraa_radxa_rock_5a_pininfo(b, 36,  4,  2, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A2");
    mraa_radxa_rock_5a_pininfo(b, 37, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "SARADC_VIN2");
    mraa_radxa_rock_5a_pininfo(b, 38,  4,  5, (mraa_pincapabilities_t){1,1,0,0,0,1,0,1}, "GPIO4_A5");
    mraa_radxa_rock_5a_pininfo(b, 39, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5a_pininfo(b, 40,  4,  9, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "GPIO4_B1");

    return b;
}
