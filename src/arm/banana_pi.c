/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Michael Ring <mail@michael-ring.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <mraa/common.h>

#include "common.h"
#include "arm/banana_pi.h"

#define PLATFORM_NAME_BANANA_PI_REV_1 "Banana Pi"
#define PLATFORM_NAME_BANANA_PRO_REV_1 "Banana Pro"
#define PLATFORM_BANANA_PI_REV_1 1
#define PLATFORM_BANANA_PRO_REV_1 2

mraa_board_t *
mraa_banana_pi() {
    int platform_detected = 0;
    mraa_board_t *b = (mraa_board_t *) malloc(sizeof(mraa_board_t));
    if (b == NULL)
        return NULL;

    // TODO: Detect Bananapi Versions, for now always PI Type
    if (1) {
        b->platform_name_length = strlen(PLATFORM_NAME_BANANA_PI_REV_1) + 1;
        b->platform_name = (char *) malloc(sizeof(char) * b->platform_name_length);
        strncpy(b->platform_name, PLATFORM_NAME_BANANA_PI_REV_1, b->platform_name_length);
        platform_detected = PLATFORM_BANANA_PI_REV_1;
        b->phy_pin_count = MRAA_BANANA_PI_REV_1_PINCOUNT;
    }
    if (0) {
        b->platform_name_length = strlen(PLATFORM_NAME_BANANA_PRO_REV_1) + 1;
        b->platform_name = (char *) malloc(sizeof(char) * b->platform_name_length);
        strncpy(b->platform_name, PLATFORM_NAME_BANANA_PRO_REV_1, b->platform_name_length);
        platform_detected = PLATFORM_BANANA_PRO_REV_1;
        b->phy_pin_count = MRAA_BANANA_PRO_REV_1_PINCOUNT;
    }

    b->aio_count = 0;
    b->gpio_count = 17;
    b->adc_raw = 0;
    b->adc_supported = 0;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t *) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);

    strncpy(b->pins[0].name, "INVALID", 8);
    b->pins[0].capabilites = (mraa_pincapabilities_t) {0, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[1].name, "3V3", 8);
    b->pins[1].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[2].name, "5V", 8);
    b->pins[2].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[3].name, "SDA.1", 8);
    b->pins[3].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0};
    b->pins[3].i2c.pinmap = 0;
    b->pins[3].i2c.mux_total = 0;

    strncpy(b->pins[4].name, "5V", 8);
    b->pins[4].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[5].name, "SCL.1", 8);
    b->pins[5].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 1, 0, 0};
    b->pins[5].i2c.pinmap = 0;
    b->pins[5].i2c.mux_total = 0;

    strncpy(b->pins[6].name, "GND", 8);
    b->pins[6].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[7].name, "PI03", 8);
    b->pins[7].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[7].gpio.pinmap = 4;
    b->pins[7].gpio.mux_total = 0;

    strncpy(b->pins[8].name, "UART_TX", 8);
    b->pins[8].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1};
    b->pins[8].uart.parent_id = 0;
    b->pins[8].uart.mux_total = 0;

    strncpy(b->pins[9].name, "GND", 8);
    b->pins[9].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[10].name, "UART_RX", 8);
    b->pins[10].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 1};
    b->pins[10].uart.parent_id = 0;
    b->pins[10].uart.mux_total = 0;

    strncpy(b->pins[11].name, "PI19", 8);
    b->pins[11].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[11].gpio.pinmap = 17;
    b->pins[11].gpio.mux_total = 0;

    strncpy(b->pins[12].name, "PH02", 8);
    b->pins[12].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[12].gpio.pinmap = 18;
    b->pins[12].gpio.mux_total = 0;

    strncpy(b->pins[13].name, "PI18", 8);
    b->pins[13].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[13].gpio.pinmap = 27;
    b->pins[13].gpio.mux_total = 0;

    strncpy(b->pins[14].name, "GND", 8);
    b->pins[14].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[15].name, "PI17", 8);
    b->pins[15].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[15].gpio.pinmap = 22;
    b->pins[15].gpio.mux_total = 0;

    strncpy(b->pins[16].name, "PH20", 8);
    b->pins[16].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[16].gpio.pinmap = 23;
    b->pins[16].gpio.mux_total = 0;

    strncpy(b->pins[17].name, "3V3", 8);
    b->pins[17].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[18].name, "PH21", 8);
    b->pins[18].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[18].gpio.pinmap = 24;
    b->pins[18].gpio.mux_total = 0;

    strncpy(b->pins[19].name, "MOSI", 8);
    b->pins[19].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0};
    b->pins[19].spi.pinmap = 0;
    b->pins[19].spi.mux_total = 0;

    strncpy(b->pins[20].name, "GND", 8);
    b->pins[20].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[21].name, "MISO", 8);
    b->pins[21].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0};
    b->pins[21].spi.pinmap = 0;
    b->pins[21].spi.mux_total = 0;

    strncpy(b->pins[22].name, "PI16", 8);
    b->pins[22].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
    b->pins[22].gpio.pinmap = 25;
    b->pins[22].gpio.mux_total = 0;

    strncpy(b->pins[23].name, "SCLK", 8);
    b->pins[23].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0};
    b->pins[23].spi.pinmap = 0;
    b->pins[23].spi.mux_total = 0;

    strncpy(b->pins[24].name, "CE0", 8);
    b->pins[24].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0};
    b->pins[24].spi.pinmap = 0;
    b->pins[24].spi.mux_total = 0;

    strncpy(b->pins[25].name, "GND", 8);
    b->pins[25].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

    strncpy(b->pins[26].name, "CE1", 8);
    b->pins[26].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 1, 0, 0, 0};
    b->pins[26].spi.pinmap = 0;
    b->pins[26].spi.mux_total = 0;

    //BUS DEFINITIONS
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = 3;
    b->i2c_bus[0].scl = 5;

    b->spi_bus_count = 2;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[0].slave_s = 0;
    b->spi_bus[0].cs = 24;
    b->spi_bus[0].mosi = 19;
    b->spi_bus[0].miso = 21;
    b->spi_bus[0].sclk = 23;

    b->spi_bus[1].bus_id = 0;
    b->spi_bus[1].slave_s = 1;
    b->spi_bus[1].cs = 26;
    b->spi_bus[1].mosi = 19;
    b->spi_bus[1].miso = 21;
    b->spi_bus[1].sclk = 23;

    b->uart_dev_count = 1;
    b->def_uart_dev = 0;
    b->uart_dev[0].rx = 10;
    b->uart_dev[0].tx = 8;

    if (platform_detected == PLATFORM_BANANA_PRO_REV_1) {
        b->gpio_count = 26;

        strncpy(b->pins[27].name, "SDA.0", 8);
        b->pins[27].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};
        strncpy(b->pins[28].name, "SCL.0", 8);
        b->pins[28].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

        //TODO
        strncpy(b->pins[29].name, "GPIO21", 8);
        b->pins[29].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[29].gpio.pinmap = 5;
        b->pins[29].gpio.mux_total = 0;

        strncpy(b->pins[30].name, "GND", 8);
        b->pins[30].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

        //TODO
        strncpy(b->pins[31].name, "GPIO22", 8);
        b->pins[31].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[31].gpio.pinmap = 6;
        b->pins[31].gpio.mux_total = 0;

        //TODO
        strncpy(b->pins[32].name, "GPIO26", 8);
        b->pins[32].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[32].gpio.pinmap = 12;
        b->pins[32].gpio.mux_total = 0;

        //TODO
        strncpy(b->pins[33].name, "GPIO23", 8);
        b->pins[33].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[33].gpio.pinmap = 13;
        b->pins[33].gpio.mux_total = 0;

        strncpy(b->pins[34].name, "GND", 8);
        b->pins[34].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

        //TODO
        strncpy(b->pins[35].name, "GPIO24", 8);
        b->pins[35].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[35].gpio.pinmap = 19;
        b->pins[35].gpio.mux_total = 0;


        //TODO
        strncpy(b->pins[36].name, "GPIO27", 8);
        b->pins[36].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[36].gpio.pinmap = 16;
        b->pins[36].gpio.mux_total = 0;

        strncpy(b->pins[37].name, "GPIO25", 8);
        b->pins[37].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[37].gpio.pinmap = 26;
        b->pins[37].gpio.mux_total = 0;

        //TODO
        strncpy(b->pins[38].name, "GPIO28", 8);
        b->pins[38].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[38].gpio.pinmap = 20;
        b->pins[38].gpio.mux_total = 0;

        strncpy(b->pins[39].name, "GND", 8);
        b->pins[39].capabilites = (mraa_pincapabilities_t) {1, 0, 0, 0, 0, 0, 0, 0};

        //TODO
        strncpy(b->pins[40].name, "GPIO29", 8);
        b->pins[40].capabilites = (mraa_pincapabilities_t) {1, 1, 0, 0, 0, 0, 0, 0};
        b->pins[40].gpio.pinmap = 21;
        b->pins[40].gpio.mux_total = 0;
    }

    return b;
}