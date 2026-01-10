/*
 * Author: Sergey Ko <u306060@gmail.com>
 * Copyright (c) Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "arm/radxa_zero_151.h"
#include "common.h"
#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

const char* radxa_zero_151_serialdev[MRAA_RADXA_ZERO_151_UART_COUNT] = { "/dev/ttyAML0", "/dev/ttyAML1", "/dev/ttyAML4" };

void
mraa_radxa_zero_pininfo(mraa_board_t* board,
                            int index,
                            int gpio_chip,
                            int gpio_line,
                            mraa_pincapabilities_t pincapabilities_t,
                            char* pin_name)
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
mraa_radxa_zero()
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
    b->phy_pin_count = MRAA_RADXA_ZERO_151_PIN_COUNT + 1;

    b->platform_name = PLATFORM_NAME_RADXA_ZERO;
    b->chardev_capable = 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_ZERO_151_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 0;       // GPIOAO-0, GPIOAO-1
    b->uart_dev[1].index = 1;       // GPIOAO-2, GPIOAO-3 / GPIOAO-8, GPIOAO-9
    b->uart_dev[2].index = 4;       // GPIOH_6, GPIOH_7, GPIOH_4, GPIOH_5
    b->uart_dev[0].device_path = (char*) radxa_zero_151_serialdev[0];
    b->uart_dev[1].device_path = (char*) radxa_zero_151_serialdev[1];
    b->uart_dev[2].device_path = (char*) radxa_zero_151_serialdev[2];

    // I2C
    b->i2c_bus_count = MRAA_RADXA_ZERO_151_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 1;       // GPIOAO-2, GPIOAO-3
    b->i2c_bus[1].bus_id = 3;       // GPIOX-10, GPIOX-11
    b->i2c_bus[2].bus_id = 4;       // GPIOH-6, GPIOH-7
    b->i2c_bus[3].bus_id = 5;       // GPIOA-14, GPIOA-15

    // SPI
    b->spi_bus_count = MRAA_RADXA_ZERO_151_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;       // GPIOX_10, GPIOX_11, GPIOX_8, GPIOX_9
    b->spi_bus[1].bus_id = 1;       // GPIOH_6, GPIOH_7, GPIOH_4, GPIOH_5

    // PWM
    b->pwm_dev_count = MRAA_RADXA_ZERO_151_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[18].pwm.parent_id = 0;  // GPIOX_8
    b->pins[18].pwm.mux_total = 0;
    b->pins[18].pwm.pinmap = 0;
    b->pins[40].pwm.parent_id = 2;  // GPIOAO_11
    b->pins[40].pwm.mux_total = 0;
    b->pins[40].pwm.pinmap = 1;

    mraa_radxa_zero_pininfo(b,  0, -1, -1, (mraa_pincapabilities_t) { 0, 0, 0, 0, 0, 0, 0, 0 }, "INVALID");
    mraa_radxa_zero_pininfo(b,  1, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "3V3");
    mraa_radxa_zero_pininfo(b,  2, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "5V");
    mraa_radxa_zero_pininfo(b,  3,  0, 72, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 1, 0, 0 }, "I2C_EE_SDA");          // GPIOA_14
    mraa_radxa_zero_pininfo(b,  4, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "5V");
    mraa_radxa_zero_pininfo(b,  5,  1,  5, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 1, 0, 0 }, "I2C_EE_SCL");          // GPIOA_15
    mraa_radxa_zero_pininfo(b,  6, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b,  7,  1,  4, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 1, 0, 1 }, "I2C_AO_SDA");          // GPIOA_3
    mraa_radxa_zero_pininfo(b,  8,  1,  0, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 0, 0, 1 }, "UART_AO_TX");          // GPIOAO_0
    mraa_radxa_zero_pininfo(b,  9, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b, 10,  1,  1, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 0, 0, 1 }, "UART_AO_RX");          // GPIOAO_1
    mraa_radxa_zero_pininfo(b, 11,  1,  2, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 1, 0, 1 }, "I2C_AO_SCL");          // GPIOAO_2
    mraa_radxa_zero_pininfo(b, 12,  0, 74, (mraa_pincapabilities_t) { 1, 1, 0, 0, 1, 0, 0, 0 }, "SPI_A_MISO");          // GPIOX_9
    mraa_radxa_zero_pininfo(b, 13,  0, 76, (mraa_pincapabilities_t) { 1, 1, 0, 0, 1, 1, 0, 0 }, "I2C_EE_SCL");          // GPIOX_11
    mraa_radxa_zero_pininfo(b, 14, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b, 15, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "SARADC_CH1");
    mraa_radxa_zero_pininfo(b, 16,  0, 75, (mraa_pincapabilities_t) { 1, 1, 0, 0, 1, 1, 0, 0 }, "I2C_EE_SDA");          // GPIOX_10
    mraa_radxa_zero_pininfo(b, 17, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "3V3");
    mraa_radxa_zero_pininfo(b, 18,  0, 73, (mraa_pincapabilities_t) { 1, 1, 1, 0, 1, 0, 0, 0 }, "SPI_A_MOSI");          // GPIOX_8
    mraa_radxa_zero_pininfo(b, 19,  0, 20, (mraa_pincapabilities_t) { 1, 1, 0, 0, 1, 0, 0, 0 }, "SPI_B_MOSI");          // GPIOH_4
    mraa_radxa_zero_pininfo(b, 20, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b, 21,  0, 21, (mraa_pincapabilities_t) { 1, 1, 0, 0, 1, 0, 0, 0 }, "SPI_B_MISO");          // GPIOH_5
    mraa_radxa_zero_pininfo(b, 22,  0, 48, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 0, 0, 0 }, "GPIOC_7");
    mraa_radxa_zero_pininfo(b, 23,  0, 23, (mraa_pincapabilities_t) { 1, 1, 0, 0, 1, 1, 0, 1 }, "I2C_EE_SCL");          // GPIOH_7
    mraa_radxa_zero_pininfo(b, 24,  0, 22, (mraa_pincapabilities_t) { 1, 1, 0, 0, 1, 1, 0, 1 }, "UART_EE_RX");          // GPIOH_6
    mraa_radxa_zero_pininfo(b, 25, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b, 26, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "SARADC_CH2");
    mraa_radxa_zero_pininfo(b, 27,  1,  3, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 1, 0, 1 }, "I2C_AO_SDA");          // GPIOAO_3
    mraa_radxa_zero_pininfo(b, 28,  1,  2, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 1, 0, 1 }, "I2C_AO_SCL");          // GPIOAO_2
    mraa_radxa_zero_pininfo(b, 29, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "NC");
    mraa_radxa_zero_pininfo(b, 30, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b, 31, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "NC");
    mraa_radxa_zero_pininfo(b, 32,  1,  4, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 0, 0, 0 }, "GPIOAO_4");
    mraa_radxa_zero_pininfo(b, 33, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "NC");
    mraa_radxa_zero_pininfo(b, 34, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b, 35,  1,  8, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 0, 0, 1 }, "UART_AO_TX");          // GPIOAO_8
    mraa_radxa_zero_pininfo(b, 36,  0, 24, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 0, 0, 0 }, "GPIOH_8");
    mraa_radxa_zero_pininfo(b, 37,  1,  9, (mraa_pincapabilities_t) { 1, 1, 0, 0, 0, 0, 0, 1 }, "UART_AO_RX");          // GPIOAO_9
    mraa_radxa_zero_pininfo(b, 38, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "NC");
    mraa_radxa_zero_pininfo(b, 39, -1, -1, (mraa_pincapabilities_t) { 1, 0, 0, 0, 0, 0, 0, 0 }, "GND");
    mraa_radxa_zero_pininfo(b, 40,  1, 11, (mraa_pincapabilities_t) { 1, 1, 1, 0, 0, 0, 0, 0 }, "PWMAO_A");             // GPIOAO_11

    return b;
}
