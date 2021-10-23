/*
 * Author: Dan O'Donovan <dan@emutex.com>
 * Copyright (c) 2015 Emutex Ltd.
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "x86/up.h"

#define PLATFORM_NAME "UP"
#define PLATFORM_VERSION "1.0.0"


static mraa_result_t
mraa_up_set_pininfo(mraa_board_t* board, int mraa_index, char* name, mraa_pincapabilities_t caps, int sysfs_pin)
{
    if (mraa_index < board->phy_pin_count) {
        mraa_pininfo_t* pin_info = &board->pins[mraa_index];
        strncpy(pin_info->name, name, MRAA_PIN_NAME_SIZE);
        pin_info->capabilities = caps;
        if (caps.gpio) {
            pin_info->gpio.pinmap = sysfs_pin;
            pin_info->gpio.mux_total = 0;
        }
        if (caps.i2c) {
            pin_info->i2c.pinmap = 1;
            pin_info->i2c.mux_total = 0;
        }
        if (caps.pwm) {
            int controller = 0;
            if (strncmp(name, "PWM", 3) == 0 && strlen(name) > 3 && isdigit(name[3]))
                controller = name[3] - '0';
            pin_info->pwm.parent_id = (unsigned int) controller;
            pin_info->pwm.pinmap = 0;
            pin_info->pwm.mux_total = 0;
        }
        if (caps.spi) {
            pin_info->spi.mux_total = 0;
        }
        if (caps.aio) {
            pin_info->aio.mux_total = 0;
            pin_info->aio.pinmap = 0;
        }
        if (caps.uart) {
            pin_info->uart.mux_total = 0;
        }
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

static mraa_result_t
mraa_up_get_pin_index(mraa_board_t* board, char* name, int* pin_index)
{
    int i;
    for (i = 0; i < board->phy_pin_count; ++i) {
        if (strncmp(name, board->pins[i].name, MRAA_PIN_NAME_SIZE) == 0) {
            *pin_index = i;
            return MRAA_SUCCESS;
        }
    }

    syslog(LOG_CRIT, "up: Failed to find pin name %s", name);

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
mraa_up_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));

    if (b == NULL) {
        return NULL;
    }

    b->platform_name = PLATFORM_NAME;
    b->platform_version = PLATFORM_VERSION;
    b->phy_pin_count = MRAA_UP_PINCOUNT;
    b->gpio_count = MRAA_UP_GPIOCOUNT;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_UP_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    b->adv_func->aio_get_valid_fp = &mraa_up_aio_get_valid_fp;

    mraa_up_set_pininfo(b, 0,  "INVALID",  (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 1,  "3.3v",     (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 2,  "5v",       (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 3,  "I2C_SDA",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 }, 2);
    mraa_up_set_pininfo(b, 4,  "5v",       (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 5,  "I2C_SCL",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 }, 3);
    mraa_up_set_pininfo(b, 6,  "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 7,  "ADC0",     (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 }, 4);
    mraa_up_set_pininfo(b, 8,  "UART_TX",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 }, 14);
    mraa_up_set_pininfo(b, 9,  "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 10, "UART_RX",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 }, 15);
    mraa_up_set_pininfo(b, 11, "UART_RTS", (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 }, 17);
    mraa_up_set_pininfo(b, 12, "I2S_CLK",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 18);
    mraa_up_set_pininfo(b, 13, "GPIO27",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 27);
    mraa_up_set_pininfo(b, 14, "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 15, "GPIO22",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 }, 22);
    mraa_up_set_pininfo(b, 16, "GPIO23",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 23);
    mraa_up_set_pininfo(b, 17, "3.3v",     (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 18, "GPIO24",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 24);
    mraa_up_set_pininfo(b, 19, "SPI_MOSI", (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 }, 10);
    mraa_up_set_pininfo(b, 20, "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 21, "SPI_MISO", (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 }, 9);
    mraa_up_set_pininfo(b, 22, "GPIO25",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 25);
    mraa_up_set_pininfo(b, 23, "SPI_CLK",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 }, 11);
    mraa_up_set_pininfo(b, 24, "SPI_CS0",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 }, 8);
    mraa_up_set_pininfo(b, 25, "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 26, "SPI_CS1",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 }, 7);
    mraa_up_set_pininfo(b, 27, "ID_SD",    (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 }, 0);
    mraa_up_set_pininfo(b, 28, "ID_SC",    (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 }, 1);
    mraa_up_set_pininfo(b, 29, "GPIO5",    (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 5);
    mraa_up_set_pininfo(b, 30, "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 31, "GPIO6",    (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 6);
    mraa_up_set_pininfo(b, 32, "PWM0",     (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, 12);
    mraa_up_set_pininfo(b, 33, "PWM1",     (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 }, 13);
    mraa_up_set_pininfo(b, 34, "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 35, "I2S_FRM",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 19);
    mraa_up_set_pininfo(b, 36, "UART_CTS", (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 }, 16);
    mraa_up_set_pininfo(b, 37, "GPIO26",   (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 26);
    mraa_up_set_pininfo(b, 38, "I2S_DIN",  (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 20);
    mraa_up_set_pininfo(b, 39, "GND",      (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 }, -1);
    mraa_up_set_pininfo(b, 40, "I2S_DOUT", (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 }, 21);

    // Set number of i2c adaptors usable from userspace
    b->i2c_bus_count = 0;
    b->def_i2c_bus = 0;
    int i2c_bus_num;

    // Configure i2c adaptor #0 (default)
    // (For consistency with Raspberry Pi 2, use I2C1 as our primary I2C bus)
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "808622C1:01", ".");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_up_get_pin_index(b, "I2C_SDA", (int*) &(b->i2c_bus[0].sda));
        mraa_up_get_pin_index(b, "I2C_SCL", (int*) &(b->i2c_bus[0].scl));
        b->i2c_bus_count++;
    }

    // Configure i2c adaptor #1
    // (normally reserved for accessing HAT EEPROM)
    i2c_bus_num = mraa_find_i2c_bus_pci("0000:00", "808622C1:00", ".");
    if (i2c_bus_num != -1) {
        int i = b->i2c_bus_count;
        b->i2c_bus[i].bus_id = i2c_bus_num;
        mraa_up_get_pin_index(b, "ID_SD", (int*) &(b->i2c_bus[1].sda));
        mraa_up_get_pin_index(b, "ID_SC", (int*) &(b->i2c_bus[1].scl));
        b->i2c_bus_count++;
    }

    // Configure PWM
    b->pwm_default_period = 500;
    b->pwm_max_period = 3413;
    b->pwm_min_period = 1;

    // Configure SPI #0 CS0 (default)
    b->spi_bus_count = 2;
    b->spi_bus[0].bus_id = 2;
    b->spi_bus[0].slave_s = 0;
    mraa_up_get_pin_index(b, "SPI_CS0", (int*) &(b->spi_bus[0].cs));
    mraa_up_get_pin_index(b, "SPI_MOSI", (int*) &(b->spi_bus[0].mosi));
    mraa_up_get_pin_index(b, "SPI_MISO", (int*) &(b->spi_bus[0].miso));
    mraa_up_get_pin_index(b, "SPI_CLK", (int*) &(b->spi_bus[0].sclk));
    b->def_spi_bus = 0;
    // Configure SPI #0 CS1
    b->spi_bus[1].bus_id = 2;
    b->spi_bus[1].slave_s = 1;
    mraa_up_get_pin_index(b, "SPI_CS1", (int*) &(b->spi_bus[1].cs));
    mraa_up_get_pin_index(b, "SPI_MOSI", (int*) &(b->spi_bus[1].mosi));
    mraa_up_get_pin_index(b, "SPI_MISO", (int*) &(b->spi_bus[1].miso));
    mraa_up_get_pin_index(b, "SPI_CLK", (int*) &(b->spi_bus[1].sclk));

    // Configure UART #1 (default)
    b->uart_dev_count = 1;
    mraa_up_get_pin_index(b, "UART_RX",  &(b->uart_dev[0].rx));
    mraa_up_get_pin_index(b, "UART_TX",  &(b->uart_dev[0].tx));
    mraa_up_get_pin_index(b, "UART_CTS", &(b->uart_dev[0].cts));
    mraa_up_get_pin_index(b, "UART_RTS", &(b->uart_dev[0].rts));
    b->uart_dev[0].device_path = "/dev/ttyS1";
    b->def_uart_dev = 0;

    // Configure ADC #0
    b->aio_count = 1;
    b->adc_raw = 8;
    b->adc_supported = 8;
    b->aio_non_seq = 1;
    mraa_up_get_pin_index(b, "ADC0", (int*) &(b->aio_dev[0].pin));

    const char* pinctrl_path = "/sys/bus/platform/drivers/up-pinctrl";
    int have_pinctrl = access(pinctrl_path, F_OK) != -1;
    syslog(LOG_NOTICE, "up: kernel pinctrl driver %savailable", have_pinctrl ? "" : "un");

    if (have_pinctrl)
        return b;

error:
    syslog(LOG_CRIT, "up: Platform failed to initialise");
    free(b);
    return NULL;
}
