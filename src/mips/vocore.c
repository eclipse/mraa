/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
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

#include "common.h"
#include "mips/vocore.h"

#define PLATFORM_NAME "VoCore"

mraa_board_t*
mraa_vocore()
{
    mraa_board_t* b = (mraa_board_t*) malloc(sizeof(mraa_board_t));
    if (b == NULL) {
        goto error;
    }

    b->platform_name_length = strlen(PLATFORM_NAME) + 1;
    b->platform_name = (char*) malloc(sizeof(char) * b->platform_name_length);
    if (b->platform_name == NULL) {
        goto error;
    }
    strncpy(b->platform_name, PLATFORM_NAME, b->platform_name_length);

    b->phy_pin_count = MRAA_VOCORE_PIN_COUNT;
    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;
    b->pwm_default_period = 1;
    b->pwm_max_period = 2;
    b->pwm_min_period = 1;
    //NO PWM supported

    b->pins = (mraa_pininfo_t*) malloc(
        sizeof(mraa_pininfo_t)*(MRAA_VOCORE_PIN_COUNT + 1));

    strncpy(b->pins[0].name, "void", 8);
    b->pins[0].capabilites = (mraa_pincapabilities_t) {0,0,0,0,0,0,0,0};
    strncpy(b->pins[1].name, "GND", 8);
    b->pins[1].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};
    strncpy(b->pins[2].name, "RESET", 8);
    b->pins[2].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};
    strncpy(b->pins[3].name, "I2C-SDA", 8);
    b->pins[3].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,1,0,0};
    b->pins[3].i2c.pinmap = 0;
    b->pins[3].i2c.mux_total = 0;
    strncpy(b->pins[4].name, "I2C-SCL", 8);
    b->pins[4].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,1,0,0};
    b->pins[4].i2c.pinmap = 0;
    b->pins[4].i2c.mux_total = 0;

    strncpy(b->pins[5].name, "G#18", 8);
    b->pins[5].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[5].gpio.pinmap = 18;
    b->pins[5].gpio.mux_total = 0;

    strncpy(b->pins[6].name, "G#00", 8);
    b->pins[6].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[6].gpio.pinmap = 0;
    b->pins[6].gpio.mux_total = 0;

    strncpy(b->pins[7].name, "G#19", 8);
    b->pins[7].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[7].gpio.pinmap = 19;
    b->pins[7].gpio.mux_total = 0;

    strncpy(b->pins[8].name, "G#20", 8);
    b->pins[8].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[8].gpio.pinmap = 20;
    b->pins[8].gpio.mux_total = 0;

    strncpy(b->pins[9].name, "G#21", 8);
    b->pins[9].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[9].gpio.pinmap = 21;
    b->pins[9].gpio.mux_total = 0;

    strncpy(b->pins[10].name, "G#17", 8);
    b->pins[10].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[10].gpio.pinmap = 17;
    b->pins[10].gpio.mux_total = 0;

    strncpy(b->pins[11].name, "GND", 8);
    b->pins[11].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};
    strncpy(b->pins[12].name, "VIN", 8);
    b->pins[12].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};
    strncpy(b->pins[13].name, "VIN", 8);
    b->pins[13].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};
    strncpy(b->pins[14].name, "GND", 8);
    b->pins[14].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};

    strncpy(b->pins[15].name, "G#12", 8);
    b->pins[15].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[15].gpio.pinmap = 12;
    b->pins[15].gpio.mux_total = 0;

    strncpy(b->pins[16].name, "G#09", 8);
    b->pins[16].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[16].gpio.pinmap = 9;
    b->pins[16].gpio.mux_total = 0;

    strncpy(b->pins[17].name, "G#22", 8);
    b->pins[17].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[17].gpio.pinmap = 22;
    b->pins[17].gpio.mux_total = 0;

    strncpy(b->pins[18].name, "G#08", 8);
    b->pins[18].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[18].gpio.pinmap = 8;
    b->pins[18].gpio.mux_total = 0;

    strncpy(b->pins[19].name, "1V8", 8);
    b->pins[19].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};

    strncpy(b->pins[20].name, "SPI-CLK", 8);
    b->pins[20].capabilites = (mraa_pincapabilities_t) {1,0,0,0,1,0,0,0};
    b->pins[20].spi.pinmap = 0;
    b->pins[20].spi.mux_total = 0;

    strncpy(b->pins[21].name, "SPI-MOSI", 8);
    b->pins[21].capabilites = (mraa_pincapabilities_t) {1,0,0,0,1,0,0,0};
    b->pins[21].spi.pinmap = 0;
    b->pins[21].spi.mux_total = 0;

    strncpy(b->pins[22].name, "SPI-CS1", 8);
    b->pins[22].capabilites = (mraa_pincapabilities_t) {1,0,0,0,1,0,0,0};
    b->pins[22].spi.pinmap = 0;
    b->pins[22].spi.mux_total = 0;

    strncpy(b->pins[23].name, "G#23", 8);
    b->pins[23].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[23].gpio.pinmap = 23;
    b->pins[23].gpio.mux_total = 0;

    strncpy(b->pins[24].name, "G#13", 8);
    b->pins[24].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[24].gpio.pinmap = 13;
    b->pins[24].gpio.mux_total = 0;

    strncpy(b->pins[25].name, "G#24", 8);
    b->pins[25].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[25].gpio.pinmap = 24;
    b->pins[25].gpio.mux_total = 0;

    strncpy(b->pins[26].name, "G#25", 8);
    b->pins[26].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[26].gpio.pinmap = 25;
    b->pins[26].gpio.mux_total = 0;

    strncpy(b->pins[27].name, "G#26", 8);
    b->pins[27].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[27].gpio.pinmap = 26;
    b->pins[27].gpio.mux_total = 0;

    strncpy(b->pins[28].name, "G#14", 8);
    b->pins[28].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[28].gpio.pinmap = 14;
    b->pins[28].gpio.mux_total = 0;

    strncpy(b->pins[29].name, "GND", 8);
    b->pins[29].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};

    strncpy(b->pins[30].name, "UARTtx", 8);
    b->pins[30].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,1};
    b->pins[30].uart.parent_id = 0;
    b->pins[30].uart.mux_total = 0;

    strncpy(b->pins[31].name, "G#07", 8);
    b->pins[31].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0,0};
    b->pins[31].gpio.pinmap = 7;
    b->pins[31].gpio.mux_total = 0;

    strncpy(b->pins[32].name, "UARTrx", 8);
    b->pins[32].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,1};
    b->pins[32].uart.parent_id = 0;
    b->pins[32].uart.mux_total = 0;

    strncpy(b->pins[33].name, "G#10", 8);
    b->pins[33].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};

    strncpy(b->pins[34].name, "SPI-MISO", 8);
    b->pins[34].capabilites = (mraa_pincapabilities_t) {1,0,0,0,1,0,0,0};
    b->pins[34].spi.pinmap = 0;
    b->pins[34].spi.mux_total = 0;

    strncpy(b->pins[35].name, "SPI-CS0", 8);
    b->pins[35].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};
    // SPI features not touched due to it being connected to the SPI storage.

    strncpy(b->pins[36].name, "G#11", 8);
    b->pins[36].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,0,0};

    //BUS DEFINITIONS
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = 3;
    b->i2c_bus[0].scl = 4;

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 32766;
    b->spi_bus[0].slave_s = 1;
    b->spi_bus[0].cs = 22;
    b->spi_bus[0].mosi = 21;
    b->spi_bus[0].miso = 34;
    b->spi_bus[0].sclk = 20;

    //Used by console
    b->uart_dev_count = 1;
    b->def_uart_dev = 0;
    b->uart_dev[0].rx = 32;
    b->uart_dev[0].tx = 30;

    return b;

error:
    syslog(LOG_CRIT, "vocore: Platform failed to initialise");
    free(b);
    return NULL;
}
