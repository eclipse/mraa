/*
 * Author: Javier Arteaga <javier@emutex.com>
 * Based on work from: Dan O'Donovan <dan@emutex.com>
 *                     Nicola Lunghi <nicola.lunghi@emutex.com>
 * Copyright (c) 2017 Emutex Ltd.
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

#include "common.h"
#include "gpio.h"
#include "x86/up2.h"

#define PLATFORM_NAME "UP2"
#define PLATFORM_VERSION "1.0.0"

#define MRAA_UP2_GPIOCOUNT   28

#define MRAA_UP2_NORTH_BASE     434
#define MRAA_UP2_NORTHWEST_BASE 357
#define MRAA_UP2_WEST_BASE      310
#define MRAA_UP2_SOUTHWEST_BASE 267

// utility function to setup pin mapping of boards
static mraa_result_t
mraa_up2_set_pininfo(mraa_board_t* board, int mraa_index, char* name,
        mraa_pincapabilities_t caps, int sysfs_pin, int chip, int line)
{
    if (mraa_index < board->phy_pin_count) {
        mraa_pininfo_t* pin_info = &board->pins[mraa_index];
        strncpy(pin_info->name, name, MRAA_PIN_NAME_SIZE);
        pin_info->capabilities = caps;
        if (caps.gpio) {
            pin_info->gpio.pinmap = sysfs_pin;
            pin_info->gpio.mux_total = 0;
            pin_info->gpio.gpio_chip = chip;
            pin_info->gpio.gpio_line = line;
        }
        if (caps.pwm) {
            pin_info->pwm.parent_id = 0;
            pin_info->pwm.pinmap = 0;
            pin_info->pwm.mux_total = 0;
        }
        if (caps.aio) {
            pin_info->aio.mux_total = 0;
            pin_info->aio.pinmap = 0;
        }
        if (caps.i2c) {
            pin_info->i2c.pinmap = 1;
            pin_info->i2c.mux_total = 0;
        }
        if (caps.spi) {
            pin_info->spi.mux_total = 0;
        }
        if (caps.uart) {
            pin_info->uart.mux_total = 0;
        }
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

static mraa_result_t
mraa_up2_get_pin_index(mraa_board_t* board, char* name, int* pin_index)
{
    int i;
    for (i = 0; i < board->phy_pin_count; ++i) {
        if (strncmp(name, board->pins[i].name, MRAA_PIN_NAME_SIZE) == 0) {
            *pin_index = i;
            return MRAA_SUCCESS;
        }
    }

    syslog(LOG_CRIT, "up2: Failed to find pin name %s", name);

    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_board_t*
mraa_up2_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof (mraa_board_t));

    if (b == NULL) {
        return NULL;
    }

    b->platform_name = PLATFORM_NAME;
    b->platform_version = PLATFORM_VERSION;
    b->phy_pin_count = MRAA_UP2_PINCOUNT;
    b->gpio_count = MRAA_UP2_GPIOCOUNT;
    b->chardev_capable = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_UP2_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t *) calloc(1, sizeof (mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    mraa_up2_set_pininfo(b, 0, "INVALID",    (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 1, "3.3v",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 2, "5v",         (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 3, "I2C_SDA",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UP2_NORTH_BASE + 28, 0, 28);
    mraa_up2_set_pininfo(b, 4, "5v",         (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 5, "I2C_SCL",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UP2_NORTH_BASE + 29, 0, 29);
    mraa_up2_set_pininfo(b, 6, "GND",        (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 7, "GPIO4",      (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 76, 1, 76);
    mraa_up2_set_pininfo(b, 8, "UART_TX",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UP2_NORTH_BASE + 43, 0, 43);
    mraa_up2_set_pininfo(b, 9, "GND",        (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 10, "UART_RX",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UP2_NORTH_BASE + 42, 0, 42);
    mraa_up2_set_pininfo(b, 11, "UART_RTS",  (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UP2_NORTH_BASE + 44, 0, 44);
    mraa_up2_set_pininfo(b, 12, "I2S_CLK",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_WEST_BASE + 16, 2, 16);
    mraa_up2_set_pininfo(b, 13, "GPIO27",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 75, 1, 75);
    mraa_up2_set_pininfo(b, 14, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 15, "GPIO22",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 74, 1, 74);
    mraa_up2_set_pininfo(b, 16, "PWM3",      (mraa_pincapabilities_t) {1, 1, 1, 0, 0, 0, 0, 0}, MRAA_UP2_NORTH_BASE + 37, 0, 37);
    mraa_up2_set_pininfo(b, 17, "3.3v",      (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 18, "GPIO24",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 48, 1, 48);
    mraa_up2_set_pininfo(b, 19, "SPI0_MOSI", (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 65, 1, 65);
    mraa_up2_set_pininfo(b, 20, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 21, "SPI0_MISO", (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 64, 1, 64);
    mraa_up2_set_pininfo(b, 22, "GPIO25",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 45, 1, 45);
    mraa_up2_set_pininfo(b, 23, "SPI0_CLK",  (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 61, 1, 61);
    mraa_up2_set_pininfo(b, 24, "SPI0_CS0",  (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 62, 1, 62);
    mraa_up2_set_pininfo(b, 25, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 26, "SPI0_CS1",  (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 63, 1, 63);
    mraa_up2_set_pininfo(b, 27, "ID_SD",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UP2_NORTH_BASE + 30, 0, 30);
    mraa_up2_set_pininfo(b, 28, "ID_SC",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UP2_NORTH_BASE + 31, 0, 31);
    mraa_up2_set_pininfo(b, 29, "GPIO5",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 73, 1, 73);
    mraa_up2_set_pininfo(b, 30, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 31, "GPIO6",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 47, 1, 47);
    mraa_up2_set_pininfo(b, 32, "PWM0",      (mraa_pincapabilities_t) {1, 1, 1, 0, 0, 0, 0, 0}, MRAA_UP2_NORTH_BASE + 34, 0, 34);
    mraa_up2_set_pininfo(b, 33, "PWM1",      (mraa_pincapabilities_t) {1, 1, 1, 0, 0, 0, 0, 0}, MRAA_UP2_NORTH_BASE + 35, 0, 35);
    mraa_up2_set_pininfo(b, 34, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 35, "I2S_FRM",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_WEST_BASE + 17, 2, 17);
    mraa_up2_set_pininfo(b, 36, "UART_CTS",  (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UP2_NORTH_BASE + 45, 0, 45);
    mraa_up2_set_pininfo(b, 37, "GPIO26",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_NORTHWEST_BASE + 46, 1, 46);
    mraa_up2_set_pininfo(b, 38, "I2S_DIN",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_WEST_BASE + 18, 2, 18);
    mraa_up2_set_pininfo(b, 39, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_up2_set_pininfo(b, 40, "I2S_DOUT",  (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UP2_WEST_BASE + 19, 2, 19);

    b->i2c_bus_count = 0;
    b->def_i2c_bus = 0;
    int i2c_bus_num;

    // Configure I2C adaptor #0 (default)
    // (For consistency with Raspberry Pi 2, use I2C1 as our primary I2C bus)
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "0000:00:16.1", "i2c_designware.1");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_up2_get_pin_index(b, "I2C_SDA", &(b->i2c_bus[i].sda));
        mraa_up2_get_pin_index(b, "I2C_SCL", &(b->i2c_bus[i].scl));
        b->i2c_bus_count++;
    }

    // Configure I2C adaptor #1
    // (normally reserved for accessing HAT EEPROM)
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "0000:00:16.0", "i2c_designware.0");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_up2_get_pin_index(b, "ID_SD", &(b->i2c_bus[i].sda));
        mraa_up2_get_pin_index(b, "ID_SC", &(b->i2c_bus[i].scl));
        b->i2c_bus_count++;
    }

    // Configure PWM
    b->pwm_dev_count = 0;
    b->def_pwm_dev = 0;
    b->pwm_default_period = 5000;
    b->pwm_max_period = 218453;
    b->pwm_min_period = 1;

    // set the correct pwm channels for pwm 1 2 3
    b->pins[32].pwm.parent_id = 0;
    b->pins[32].pwm.pinmap = 0;
    b->pwm_dev_count++;
    b->pins[33].pwm.parent_id = 0;
    b->pins[33].pwm.pinmap = 1;
    b->pwm_dev_count++;
    b->pins[16].pwm.parent_id = 0;
    b->pins[16].pwm.pinmap = 3;
    b->pwm_dev_count++;

    // Configure SPI
    b->spi_bus_count = 0;
    b->def_spi_bus = 0;

    // Configure SPI #0 CS0 (default)
    b->spi_bus[0].bus_id = 1;
    b->spi_bus[0].slave_s = 0;
    mraa_up2_get_pin_index(b, "SPI0_CS0",  &(b->spi_bus[0].cs));
    mraa_up2_get_pin_index(b, "SPI0_MOSI", &(b->spi_bus[0].mosi));
    mraa_up2_get_pin_index(b, "SPI0_MISO", &(b->spi_bus[0].miso));
    mraa_up2_get_pin_index(b, "SPI0_CLK",  &(b->spi_bus[0].sclk));
    b->spi_bus_count++;

    // Configure SPI #0 CS1
    b->spi_bus[1].bus_id = 1;
    b->spi_bus[1].slave_s = 1;
    mraa_up2_get_pin_index(b, "SPI0_CS1",  &(b->spi_bus[1].cs));
    mraa_up2_get_pin_index(b, "SPI0_MOSI", &(b->spi_bus[1].mosi));
    mraa_up2_get_pin_index(b, "SPI0_MISO", &(b->spi_bus[1].miso));
    mraa_up2_get_pin_index(b, "SPI0_CLK",  &(b->spi_bus[1].sclk));
    b->spi_bus_count++;

    // Configure UART
    b->uart_dev_count = 0;
    b->def_uart_dev = 0;
    // setting up a default path
    if (mraa_find_uart_bus_pci("/sys/bus/pci/devices/0000:00:18.1/dw-apb-uart.9/tty/",
                               &(b->uart_dev[0].device_path)) != MRAA_SUCCESS) {
        goto error;
    }

    // Configure UART #1 (default)
    mraa_up2_get_pin_index(b, "UART_RX", &(b->uart_dev[0].rx));
    mraa_up2_get_pin_index(b, "UART_TX", &(b->uart_dev[0].tx));
    mraa_up2_get_pin_index(b, "UART_CTS", &(b->uart_dev[0].cts));
    mraa_up2_get_pin_index(b, "UART_RTS", &(b->uart_dev[0].rts));
    b->uart_dev_count++;

    // Configure ADCs
    b->aio_count = 0;

    const char* pinctrl_path = "/sys/bus/platform/drivers/upboard-pinctrl";
    int have_pinctrl = access(pinctrl_path, F_OK) != -1;
    syslog(LOG_NOTICE, "up2: kernel pinctrl driver %savailable", have_pinctrl ? "" : "un");

    if (have_pinctrl)
        return b;

error:
    syslog(LOG_CRIT, "up2: Platform failed to initialise");
    free(b);
    return NULL;
}
