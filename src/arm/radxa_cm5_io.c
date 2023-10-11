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

#include "arm/radxa_cm5_io.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"

const char* radxa_cm5_io_serialdev[MRAA_RADXA_CM5_IO_UART_COUNT] = { "/dev/ttyS2", "/dev/ttyS3"};

void
mraa_radxa_cm5_io_pininfo(mraa_board_t* board, int index, int gpio_chip, int gpio_line, mraa_pincapabilities_t pincapabilities_t, char* pin_name)
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
mraa_radxa_cm5_io()
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
    b->phy_pin_count = MRAA_RADXA_CM5_IO_PIN_COUNT + 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_CM5_IO_UART_COUNT;
    b->platform_name = PLATFORM_NAME_RADXA_CM5_IO;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 2;
    b->uart_dev[1].index = 3;
    b->uart_dev[0].device_path = (char*) radxa_cm5_io_serialdev[0];
    b->uart_dev[1].device_path = (char*) radxa_cm5_io_serialdev[1];

    // I2C
    b->i2c_bus_count = MRAA_RADXA_CM5_IO_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 1;
    b->i2c_bus[1].bus_id = 3;
    b->i2c_bus[2].bus_id = 5;
    b->i2c_bus[3].bus_id = 7;

    // SPI
    b->spi_bus_count = MRAA_RADXA_CM5_IO_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;

    // PWM
    b->pwm_dev_count = MRAA_RADXA_CM5_IO_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[38].pwm.parent_id = 0;    // PWM0-M1
    b->pins[38].pwm.mux_total = 0;
    b->pins[38].pwm.pinmap = 0;
    b->pins[31].pwm.parent_id = 6;    // PWM6-M0
    b->pins[31].pwm.mux_total = 0;
    b->pins[31].pwm.pinmap = 0;
    b->pins[29].pwm.parent_id = 7;   // PWM7-M0
    b->pins[29].pwm.mux_total = 0;
    b->pins[29].pwm.pinmap = 0;
    b->pins[16].pwm.parent_id = 11;   // PWM11-M0
    b->pins[16].pwm.mux_total = 0;
    b->pins[16].pwm.pinmap = 0;
    b->pins[32].pwm.parent_id = 13;  // PWM13-M2
    b->pins[32].pwm.mux_total = 0;
    b->pins[32].pwm.pinmap = 0;
    b->pins[24].pwm.parent_id = 11;   // PWM14-M1
    b->pins[24].pwm.mux_total = 0;
    b->pins[24].pwm.pinmap = 0;
    b->pins[36].pwm.parent_id = 15;   // PWM15-M2
    b->pins[36].pwm.mux_total = 0;
    b->pins[36].pwm.pinmap = 0;

    // AIO
    b->aio_count = MRAA_RADXA_CM5_IO_AIO_COUNT;
    b->adc_raw = 10;
    b->adc_supported = 10;
    b->aio_dev[0].pin = 37;
    b->aio_non_seq = 1;
    b->chardev_capable = 1;

    mraa_radxa_cm5_io_pininfo(b, 0, -1, -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_radxa_cm5_io_pininfo(b, 1, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_cm5_io_pininfo(b, 2, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_cm5_io_pininfo(b, 3, 3, 27, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "GPIO3_D3");
    mraa_radxa_cm5_io_pininfo(b, 4, -1, -1, (mraa_pincapabilities_t){1,0,0,0, 0,0,0,0}, "5V");
    mraa_radxa_cm5_io_pininfo(b, 5, 3, 26, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "GPIO3_D2");
    mraa_radxa_cm5_io_pininfo(b, 6, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 7, 4, 7, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "GPIO4_A7");
    mraa_radxa_cm5_io_pininfo(b, 8, 4, 13, (mraa_pincapabilities_t){1,0,0,0,0,1,0,1}, "GPIO0_B5");  // IO resources is occupied by uart, function GPIO can't be used.
    mraa_radxa_cm5_io_pininfo(b, 9, -1, -1, (mraa_pincapabilities_t){1,0,0,0, 0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 10, 0, 14, (mraa_pincapabilities_t){1,0,0,0,0,1,0,1}, "GPIO0_B6");  // IO resources is occupied by uart, function GPIO can't be used.
    mraa_radxa_cm5_io_pininfo(b, 11, 4, 6, (mraa_pincapabilities_t){1,1,0,0,0,1,0,1}, "GPIO4_A6");
    mraa_radxa_cm5_io_pininfo(b, 12, 0, 18, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO0_C2");
    mraa_radxa_cm5_io_pininfo(b, 13, 4, 5, (mraa_pincapabilities_t){1,1,0,0,0,1,0,1}, "GPIO4_A5");
    mraa_radxa_cm5_io_pininfo(b, 14, -1, -1, (mraa_pincapabilities_t){1,0,0,0, 0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 15, 4, 4, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "GPIO4_A4");
    mraa_radxa_cm5_io_pininfo(b, 16, 1, 20, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO1_C4");
    mraa_radxa_cm5_io_pininfo(b, 17, -1, -1, (mraa_pincapabilities_t){1,0,0,0, 0,0,0,0}, "3V3");
    mraa_radxa_cm5_io_pininfo(b, 18, 1, 29, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO1_D5");
    mraa_radxa_cm5_io_pininfo(b, 19, 4, 1, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A1");
    mraa_radxa_cm5_io_pininfo(b, 20, -1, -1, (mraa_pincapabilities_t){1,0,0,0, 0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 21, 4, 0, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A0");
    mraa_radxa_cm5_io_pininfo(b, 22, 1, 9, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO1_B1");
    mraa_radxa_cm5_io_pininfo(b, 23, 4, 2, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO4_A2");
    mraa_radxa_cm5_io_pininfo(b, 24, 4, 10, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO4_B2");
    mraa_radxa_cm5_io_pininfo(b, 25, -1, -1, (mraa_pincapabilities_t){1,0,0,0, 0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 26, 3, 15, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "GPIO3_B7");
    mraa_radxa_cm5_io_pininfo(b, 27, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_cm5_io_pininfo(b, 28, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_cm5_io_pininfo(b, 29, 0, 24, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO0_D0");
    mraa_radxa_cm5_io_pininfo(b, 30, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 31, 0, 23, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO0_C7");
    mraa_radxa_cm5_io_pininfo(b, 32, 1, 15, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO1_B7");
    mraa_radxa_cm5_io_pininfo(b, 33, 1, 17, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "GPIO1_C1");
    mraa_radxa_cm5_io_pininfo(b, 34, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 35, 3, 16, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "GPIO3_C0");
    mraa_radxa_cm5_io_pininfo(b, 36, 1, 22, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO1_C6");
    mraa_radxa_cm5_io_pininfo(b, 37, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "SARADC_VIN4");
    mraa_radxa_cm5_io_pininfo(b, 38, 1, 26, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "GPIO1_D2");
    mraa_radxa_cm5_io_pininfo(b, 39, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_cm5_io_pininfo(b, 40, 0, 27, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO0_D3");

    return b;
}
