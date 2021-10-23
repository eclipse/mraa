/*
 * Author: Michael Campion <michael.campion@emutex.com>
 * Based on work from: Dan O'Donovan <dan@emutex.com>
 *                     Nicola Lunghi <nicola.lunghi@emutex.com>
 * Copyright (c) 2019 Emutex Ltd.
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
#include "x86/up_xtreme.h"

#define PLATFORM_NAME "UPXTREME"
#define PLATFORM_VERSION "1.0.0"

#define MRAA_UPXTREME_GPIOCOUNT 28

#define MRAA_UPXTREME_CHIP_BASE 200

// utility function to setup pin mapping of boards
static mraa_result_t
mraa_upxtreme_set_pininfo(mraa_board_t* board, int mraa_index, char* name,
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
mraa_upxtreme_get_pin_index(mraa_board_t* board, char* name, int* pin_index)
{
    int i;
    for (i = 0; i < board->phy_pin_count; ++i) {
        if (strncmp(name, board->pins[i].name, MRAA_PIN_NAME_SIZE) == 0) {
            *pin_index = i;
            return MRAA_SUCCESS;
        }
    }

    syslog(LOG_CRIT, "UP Xtreme: Failed to find pin name %s", name);

    return MRAA_ERROR_INVALID_RESOURCE;
}

static mraa_result_t
mraa_up_aio_get_valid_fp(mraa_aio_context dev)
{
    char file_path[64] = "";

    /*
     * Open file Analog device input channel raw voltage file for reading.
     *
     * The UP ADC has only 1 channel, so the channel number is not included
     * in the filename
     */
    snprintf(file_path, 64, "/sys/bus/iio/devices/iio:device0/in_voltage_raw");

    dev->adc_in_fp = open(file_path, O_RDONLY);
    if (dev->adc_in_fp == -1) {
        syslog(LOG_ERR, "aio: Failed to open input raw file %s for reading!", file_path);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_upxtreme_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof (mraa_board_t));

    if (b == NULL) {
        return NULL;
    }

    b->platform_name = PLATFORM_NAME;
    b->platform_version = PLATFORM_VERSION;
    b->phy_pin_count = MRAA_UPXTREME_PINCOUNT;
    b->gpio_count = MRAA_UPXTREME_GPIOCOUNT;
    b->chardev_capable = 0;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_UPXTREME_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t *) calloc(1, sizeof (mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    b->adv_func->aio_get_valid_fp = &mraa_up_aio_get_valid_fp;

    mraa_upxtreme_set_pininfo(b, 0, "INVALID",    (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 1, "3.3v",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 2, "5v",         (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 3, "I2C_SDA",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 168, 0, 168);
    mraa_upxtreme_set_pininfo(b, 4, "5v",         (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 5, "I2C_SCL",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 169, 0, 169);
    mraa_upxtreme_set_pininfo(b, 6, "GND",        (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 7, "ADC0",       (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 1, 0}, MRAA_UPXTREME_CHIP_BASE + 96, 0, 96);
    mraa_upxtreme_set_pininfo(b, 8, "UART_TX",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UPXTREME_CHIP_BASE + 269, 0, 269);
    mraa_upxtreme_set_pininfo(b, 9, "GND",        (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 10, "UART_RX",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UPXTREME_CHIP_BASE + 268, 0, 268);
    mraa_upxtreme_set_pininfo(b, 11, "UART_RTS",  (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UPXTREME_CHIP_BASE + 270, 0, 270);
    mraa_upxtreme_set_pininfo(b, 12, "I2S_CLK",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 160, 0, 160);
    mraa_upxtreme_set_pininfo(b, 13, "GPIO27",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 68, 0, 68);
    mraa_upxtreme_set_pininfo(b, 14, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 15, "GPIO22",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 70, 0, 70);
    mraa_upxtreme_set_pininfo(b, 16, "GPIO23",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 64, 0, 64);
    mraa_upxtreme_set_pininfo(b, 17, "3.3v",      (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 18, "GPIO24",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 65, 0, 65);
    mraa_upxtreme_set_pininfo(b, 19, "SPI0_MOSI", (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 108, 0, 108);
    mraa_upxtreme_set_pininfo(b, 20, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 21, "SPI0_MISO", (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 107, 0, 107);
    mraa_upxtreme_set_pininfo(b, 22, "GPIO25",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 66, 0, 66);
    mraa_upxtreme_set_pininfo(b, 23, "SPI0_CLK",  (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 106, 0, 106);
    mraa_upxtreme_set_pininfo(b, 24, "SPI0_CS0",  (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 105, 0, 105);
    mraa_upxtreme_set_pininfo(b, 25, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 26, "SPI0_CS1",  (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 111, 0, 111);
    mraa_upxtreme_set_pininfo(b, 27, "ID_SD",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 166, 0, 166);
    mraa_upxtreme_set_pininfo(b, 28, "ID_SC",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 167, 0, 167);
    mraa_upxtreme_set_pininfo(b, 29, "GPIO5",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 117, 0, 117);
    mraa_upxtreme_set_pininfo(b, 30, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 31, "GPIO6",     (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 118, 0, 118);
    mraa_upxtreme_set_pininfo(b, 32, "GPIO12",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 98, 0, 98);
    mraa_upxtreme_set_pininfo(b, 33, "GPIO13",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 99, 0, 99);
    mraa_upxtreme_set_pininfo(b, 34, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 35, "I2S_FRM",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 161, 0, 161);
    mraa_upxtreme_set_pininfo(b, 36, "UART_CTS",  (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1}, MRAA_UPXTREME_CHIP_BASE + 271, 0, 271);
    mraa_upxtreme_set_pininfo(b, 37, "GPIO26",    (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 67, 0, 67);
    mraa_upxtreme_set_pininfo(b, 38, "I2S_DIN",   (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 163, 0, 163);
    mraa_upxtreme_set_pininfo(b, 39, "GND",       (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0}, -1, -1, -1);
    mraa_upxtreme_set_pininfo(b, 40, "I2S_DOUT",  (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0}, MRAA_UPXTREME_CHIP_BASE + 162, 0, 162);

    b->i2c_bus_count = 0;
    b->def_i2c_bus = 0;
    int i2c_bus_num;

    // Configure I2C adaptor #0 (default)
    // (For consistency with Raspberry Pi 2, use I2C1 as our primary I2C bus)
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "0000:00:19.0", "i2c_designware.3");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_upxtreme_get_pin_index(b, "I2C_SDA", &(b->i2c_bus[i].sda));
        mraa_upxtreme_get_pin_index(b, "I2C_SCL", &(b->i2c_bus[i].scl));
        b->i2c_bus_count++;
    } else {
        syslog(LOG_WARNING, "UP Xtreme: Platform failed to find I2C0 controller");
    }

    // Configure I2C adaptor #1
    // (normally reserved for accessing HAT EEPROM)
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "0000:00:15.3", "i2c_designware.2");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_upxtreme_get_pin_index(b, "ID_SD", &(b->i2c_bus[i].sda));
        mraa_upxtreme_get_pin_index(b, "ID_SC", &(b->i2c_bus[i].scl));
        b->i2c_bus_count++;
    } else {
        syslog(LOG_WARNING, "UP Xtreme: Platform failed to find I2C1 controller");
    }

    // Configure PWM
    b->pwm_dev_count = 0;

    // Configure SPI
    b->spi_bus_count = 0;
    b->def_spi_bus = 0;

    // Configure SPI #0 CS0 (default)
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[0].slave_s = 0;
    mraa_upxtreme_get_pin_index(b, "SPI0_CS0",  &(b->spi_bus[0].cs));
    mraa_upxtreme_get_pin_index(b, "SPI0_MOSI", &(b->spi_bus[0].mosi));
    mraa_upxtreme_get_pin_index(b, "SPI0_MISO", &(b->spi_bus[0].miso));
    mraa_upxtreme_get_pin_index(b, "SPI0_CLK",  &(b->spi_bus[0].sclk));
    b->spi_bus_count++;

    // Configure SPI #0 CS1
    b->spi_bus[1].bus_id = 0;
    b->spi_bus[1].slave_s = 1;
    mraa_upxtreme_get_pin_index(b, "SPI0_CS1",  &(b->spi_bus[1].cs));
    mraa_upxtreme_get_pin_index(b, "SPI0_MOSI", &(b->spi_bus[1].mosi));
    mraa_upxtreme_get_pin_index(b, "SPI0_MISO", &(b->spi_bus[1].miso));
    mraa_upxtreme_get_pin_index(b, "SPI0_CLK",  &(b->spi_bus[1].sclk));
    b->spi_bus_count++;

    // Configure UART
    b->uart_dev_count = 0;
    b->def_uart_dev = 0;
    // setting up a default path
    if (mraa_find_uart_bus_pci("/sys/bus/pci/devices/0000:00:1e.1/dw-apb-uart.6/tty/",
                               &(b->uart_dev[0].device_path)) != MRAA_SUCCESS) {
        syslog(LOG_WARNING, "UP Xtreme: Platform failed to find uart controller");
    } else {
        // Configure UART #1 (default)
        mraa_upxtreme_get_pin_index(b, "UART_RX", &(b->uart_dev[0].rx));
        mraa_upxtreme_get_pin_index(b, "UART_TX", &(b->uart_dev[0].tx));
        mraa_upxtreme_get_pin_index(b, "UART_CTS", &(b->uart_dev[0].cts));
        mraa_upxtreme_get_pin_index(b, "UART_RTS", &(b->uart_dev[0].rts));
        b->uart_dev_count++;
    }

    // Configure ADC #0
    b->aio_count = 1;
    b->adc_raw = 8;
    b->adc_supported = 8;
    b->aio_non_seq = 1;
    mraa_upxtreme_get_pin_index(b, "ADC0", (int*) &(b->aio_dev[0].pin));

    const char* pinctrl_path = "/sys/bus/platform/drivers/upboard-pinctrl";
    int have_pinctrl = access(pinctrl_path, F_OK) != -1;
    syslog(LOG_NOTICE, "UP Xtreme: kernel pinctrl driver %savailable", have_pinctrl ? "" : "un");

    if (have_pinctrl)
        return b;

error:
    syslog(LOG_CRIT, "UP Xtreme: Platform failed to initialise");
    free(b);
    return NULL;
}
