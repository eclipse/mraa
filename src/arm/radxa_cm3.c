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
#include "arm/radxa_cm3.h"
#include "common.h"

const char* radxa_cm3_serialdev[MRAA_RADXA_CM3_UART_COUNT] = { "/dev/ttyS2" };

void
mraa_radxa_cm3_pininfo(mraa_board_t* board, int index, int gpio_chip, int gpio_line, mraa_pincapabilities_t pincapabilities_t, char* pin_name)
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
mraa_radxa_cm3()
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
    b->phy_pin_count = MRAA_RADXA_CM3_PIN_COUNT + 1;

    if (mraa_file_contains("/proc/device-tree/model", PLATFORM_NAME_RADXA_CM3_IO) ||
        mraa_file_contains("/proc/device-tree/model", PLATFORM_NAME_RADXA_CM3_IO_2)) {
        b->platform_name = PLATFORM_NAME_RADXA_CM3_IO_2;
    } else if (mraa_file_contains("/proc/device-tree/model", PLATFORM_NAME_RADXA_CM3_RPI_CM4_IO)) {
        b->platform_name = PLATFORM_NAME_RADXA_CM3_RPI_CM4_IO;
    } else {
        printf("An unknown product detected. Fail early...\n");
        exit(-1);
    }

    b->chardev_capable = 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_CM3_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 2;
    b->uart_dev[0].device_path = (char*) radxa_cm3_serialdev[0];

    // I2C
    b->i2c_bus_count = MRAA_RADXA_CM3_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 2;
    b->i2c_bus[1].bus_id = 4;

    // SPI
    b->spi_bus_count = MRAA_RADXA_CM3_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[1].bus_id = 3;

    // PWM
    b->pwm_dev_count = MRAA_RADXA_CM3_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[13].pwm.parent_id = 0;  // pwm0-m0
    b->pins[13].pwm.mux_total = 0;
    b->pins[11].pwm.parent_id = 0;  // pwm0-m1
    b->pins[11].pwm.mux_total = 0;
    b->pins[5].pwm.parent_id = 1; // pwm1-m1
    b->pins[5].pwm.mux_total = 0;
    b->pins[3].pwm.parent_id = 2; // pwm2-m1
    b->pins[3].pwm.mux_total = 0;
    b->pins[37].pwm.parent_id = 3; // pwm3
    b->pins[37].pwm.mux_total = 0;
    b->pins[15].pwm.parent_id = 4;  // pwm4
    b->pins[15].pwm.mux_total = 0;
    b->pins[31].pwm.parent_id = 6; // pwm6
    b->pins[31].pwm.mux_total = 0;
    b->pins[33].pwm.parent_id = 15; // pwm7
    b->pins[33].pwm.mux_total = 0;
    b->pins[32].pwm.parent_id = 11; // pwm11-m1
    b->pins[32].pwm.mux_total = 0;

    mraa_radxa_cm3_pininfo(b, 0, -1, -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_radxa_cm3_pininfo(b, 1, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3.3V");
    mraa_radxa_cm3_pininfo(b, 2, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5.0V");
    mraa_radxa_cm3_pininfo(b, 3, 0, 14, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO0_B6");
    mraa_radxa_cm3_pininfo(b, 4, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5.0V");
    mraa_radxa_cm3_pininfo(b, 5, 0, 13, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO0_B5");
    mraa_radxa_cm3_pininfo(b, 6, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 7, 3, 29, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_D5");
    mraa_radxa_cm3_pininfo(b, 8, 0, 25, (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, "GPIO0_D1"); // GPIO0_D1 was used by fiq_debugger, function GPIO cannot be enabled
    mraa_radxa_cm3_pininfo(b, 9, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 10, 0, 24, (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, "GPIO0_D0"); // GPIO0_D0 was used by fiq_debugger, function GPIO cannot be enabled
    mraa_radxa_cm3_pininfo(b, 11, 0, 23, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO0_C7");
    mraa_radxa_cm3_pininfo(b, 12, 3, 23, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_C7");
    mraa_radxa_cm3_pininfo(b, 13, 0 ,15, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO0_B7");
    mraa_radxa_cm3_pininfo(b, 14, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 15, 0, 19, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO0_C3");
    mraa_radxa_cm3_pininfo(b, 16, 3, 28, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_D4");
    mraa_radxa_cm3_pininfo(b, 17, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3.3V");
    mraa_radxa_cm3_pininfo(b, 18, 3, 27, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_D3");
    mraa_radxa_cm3_pininfo(b, 19, 4, 10, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "GPIO4_B2");
    mraa_radxa_cm3_pininfo(b, 20, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 21, 4, 8, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_B0");
    mraa_radxa_cm3_pininfo(b, 22, 3, 22, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_C6");
    mraa_radxa_cm3_pininfo(b, 23, 4, 11, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "GPIO4_B3");
    mraa_radxa_cm3_pininfo(b, 24, 4, 6, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A6");
    mraa_radxa_cm3_pininfo(b, 25, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 26, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "SARADC_VIN3");
    mraa_radxa_cm3_pininfo(b, 27, 4, 12, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "GPIO4_B4");
    mraa_radxa_cm3_pininfo(b, 28, 4, 13, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "GPIO4_B5");
    mraa_radxa_cm3_pininfo(b, 29, 4, 9, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_B1");
    mraa_radxa_cm3_pininfo(b, 30, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 31, 0, 21, (mraa_pincapabilities_t){1,1,1,0,1,0,0,0}, "GPIO0_C5");
    mraa_radxa_cm3_pininfo(b, 32, 4, 16, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO4_C0");
    mraa_radxa_cm3_pininfo(b, 33, 0, 22, (mraa_pincapabilities_t){1,1,1,0,1,0,0,0}, "GPIO0_C6");
    mraa_radxa_cm3_pininfo(b, 34, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 35, 3, 24, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_D0");
    mraa_radxa_cm3_pininfo(b, 36, 4, 7, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A7");
    mraa_radxa_cm3_pininfo(b, 37, 0, 18, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO0_C2"); 
    mraa_radxa_cm3_pininfo(b, 38, 3, 26, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_D2");
    mraa_radxa_cm3_pininfo(b, 39, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm3_pininfo(b, 40, 3, 25, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_D1");

    return b;
}
