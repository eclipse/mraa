/*
 * Author: Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "x86/iei_tank.h"

#define PLATFORM_NAME "IEI Tank"

mraa_board_t*
mraa_iei_tank()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = MRAA_IEI_TANK_PINCOUNT;
    b->chardev_capable = 1;

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        goto error;
    }

    b->pins = (mraa_pininfo_t*) calloc(MRAA_IEI_TANK_PINCOUNT,sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b->adv_func);
        goto error;
    }

    // Maps the DB9 DIO connector
    strncpy(b->pins[0].name, "DIN0", 8);
    b->pins[0].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[0].gpio.pinmap = 4;
    b->pins[0].gpio.mux_total = 0;
    b->pins[0].gpio.gpio_chip = 0;
    b->pins[0].gpio.gpio_line = 4;
    strncpy(b->pins[1].name, "DOUT0", 8);
    b->pins[1].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[1].gpio.pinmap = 0;
    b->pins[1].gpio.mux_total = 0;
    b->pins[1].gpio.gpio_chip = 0;
    b->pins[1].gpio.gpio_line = 0;
    strncpy(b->pins[2].name, "DIN1", 8);
    b->pins[2].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[2].gpio.pinmap = 11;
    b->pins[2].gpio.mux_total = 0;
    b->pins[2].gpio.gpio_chip = 1;
    b->pins[2].gpio.gpio_line = 1;
    strncpy(b->pins[3].name, "DOUT1", 8);
    b->pins[3].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[3].gpio.pinmap = 1;
    b->pins[3].gpio.mux_total = 0;
    b->pins[3].gpio.gpio_chip = 0;
    b->pins[3].gpio.gpio_line = 1;
    strncpy(b->pins[4].name, "DIN2", 8);
    b->pins[4].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[4].gpio.pinmap = 12;
    b->pins[4].gpio.mux_total = 0;
    b->pins[4].gpio.gpio_chip = 1;
    b->pins[4].gpio.gpio_line = 2;
    strncpy(b->pins[5].name, "DOUT2", 8);
    b->pins[5].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[5].gpio.pinmap = 2;
    b->pins[5].gpio.mux_total = 0;
    b->pins[5].gpio.gpio_chip = 0;
    b->pins[5].gpio.gpio_line = 2;
    strncpy(b->pins[6].name, "DIN3", 8);
    b->pins[6].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[6].gpio.pinmap = 13;
    b->pins[6].gpio.mux_total = 0;
    b->pins[6].gpio.gpio_chip = 1;
    b->pins[6].gpio.gpio_line = 3;
    strncpy(b->pins[7].name, "DOUT3", 8);
    b->pins[7].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[7].gpio.pinmap = 3;
    b->pins[7].gpio.mux_total = 0;
    b->pins[7].gpio.gpio_chip = 0;
    b->pins[7].gpio.gpio_line = 3;
    strncpy(b->pins[8].name, "5V", 8);
    b->pins[8].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // Maps the 6 uart ports
    b->uart_dev_count = 6;
    b->def_uart_dev = 0;
    if (mraa_find_uart_bus_pci("/sys/devices/pnp0/00:03/tty",
        &(b->uart_dev[0].device_path)) != MRAA_SUCCESS) {
        syslog(LOG_WARNING, "Unable to initialize COM1");
        b->uart_dev_count--;
    } else {
        b->uart_dev[0].name = "COM1";
        b->uart_dev[0].index = 0;
    }
    if (mraa_find_uart_bus_pci("/sys/devices/pnp0/00:04/tty",
        &(b->uart_dev[1].device_path)) != MRAA_SUCCESS) {
        syslog(LOG_WARNING, "Unable to initialize COM2");
        b->uart_dev_count--;
    } else {
        b->uart_dev[1].name = "COM2";
        b->uart_dev[1].index = 1;
    }
    if (mraa_find_uart_bus_pci("/sys/devices/pnp0/00:05/tty",
        &(b->uart_dev[2].device_path)) != MRAA_SUCCESS) {
        syslog(LOG_WARNING, "Unable to initialize COM3");
        b->uart_dev_count--;
    } else {
        b->uart_dev[2].name = "COM3";
        b->uart_dev[2].index = 2;
    }
    if (mraa_find_uart_bus_pci("/sys/devices/pnp0/00:06/tty",
        &(b->uart_dev[3].device_path)) != MRAA_SUCCESS) {
        syslog(LOG_WARNING, "Unable to initialize COM4");
        b->uart_dev_count--;
    } else {
        b->uart_dev[3].name = "COM4";
        b->uart_dev[3].index = 3;
    }
    if (mraa_find_uart_bus_pci("/sys/devices/pnp0/00:07/tty",
        &(b->uart_dev[4].device_path)) != MRAA_SUCCESS) {
        syslog(LOG_WARNING, "Unable to initialize COM5");
        b->uart_dev_count--;
    } else {
        b->uart_dev[4].name = "COM5";
        b->uart_dev[4].index = 4;
    }
    if (mraa_find_uart_bus_pci("/sys/devices/pnp0/00:08/tty",
        &(b->uart_dev[5].device_path)) != MRAA_SUCCESS) {
        syslog(LOG_WARNING, "Unable to initialize COM6");
        b->uart_dev_count--;
    } else {
        b->uart_dev[5].name = "COM6";
        b->uart_dev[5].index = 5;
    }

    return b;
error:
    syslog(LOG_CRIT, "iei-tank: Platform failed to initialize");
    free(b);
    return NULL;
}
