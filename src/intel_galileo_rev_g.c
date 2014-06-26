/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
#include "intel_galileo_rev_g.h"

mraa_board_t*
mraa_intel_galileo_gen2()
{
    mraa_board_t* b = (mraa_board_t*) malloc(sizeof(mraa_board_t));
    if (b == NULL)
        return NULL;

    b->phy_pin_count = 20;
    b->gpio_count = 14;
    b->aio_count = 6;
    b->adc_raw = 12;
    b->adc_supported = 10;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t)*MRAA_INTEL_GALILEO_GEN_2_PINCOUNT);

    strncpy(b->pins[0].name, "IO0", 8);
    b->pins[0].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[0].gpio.pinmap = 11;
    b->pins[0].gpio.parent_id = 0;
    b->pins[0].gpio.mux_total = 0;
    b->pins[0].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[0].gpio.output_enable = 32;
    b->pins[0].gpio.pullup_enable = 33;

    strncpy(b->pins[1].name, "IO1", 8);
    b->pins[1].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[1].gpio.pinmap = 12;
    b->pins[1].gpio.parent_id = 0;
    b->pins[1].gpio.mux_total = 1;
    b->pins[1].gpio.mux[0].pin = 45;
    b->pins[1].gpio.mux[0].value = 0;
    b->pins[1].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[1].gpio.output_enable = 28;
    b->pins[1].gpio.pullup_enable = 29;

    strncpy(b->pins[2].name, "IO2", 8);
    b->pins[2].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[2].gpio.pinmap = 13;
    b->pins[2].gpio.parent_id = 0;
    b->pins[2].gpio.mux_total = 1;
    b->pins[2].gpio.mux[0].pin = 77;
    b->pins[2].gpio.mux[0].value = 0;
    b->pins[2].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[2].gpio.output_enable = 34;
    b->pins[2].gpio.pullup_enable = 35;

    strncpy(b->pins[3].name, "IO3", 8);
    b->pins[3].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0};
    b->pins[3].gpio.pinmap = 14;
    b->pins[3].gpio.parent_id = 0;
    b->pins[3].gpio.mux_total = 2;
    b->pins[3].gpio.mux[0].pin = 76;
    b->pins[3].gpio.mux[0].value = 0;
    b->pins[3].gpio.mux[1].pin = 64;
    b->pins[3].gpio.mux[1].value = 0;
    b->pins[3].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[3].gpio.output_enable = 16;
    b->pins[3].gpio.pullup_enable = 17;
    b->pins[3].pwm.pinmap = 1;
    b->pins[3].pwm.parent_id = 0;
    b->pins[3].pwm.mux_total = 3;
    b->pins[3].pwm.mux[0].pin = 76;
    b->pins[3].pwm.mux[0].value = 0;
    b->pins[3].pwm.mux[1].pin = 64;
    b->pins[3].pwm.mux[1].value = 1;
    b->pins[3].pwm.mux[2].pin = 16;
    b->pins[3].pwm.mux[2].value = 0;
   //ADD Othher Bits?

    strncpy(b->pins[4].name, "IO4", 8);
    b->pins[4].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[4].gpio.pinmap = 6;
    b->pins[4].gpio.parent_id = 0;
    b->pins[4].gpio.mux_total = 0;
    b->pins[4].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[4].gpio.output_enable = 36;
    b->pins[4].gpio.pullup_enable = 37;

    strncpy(b->pins[5].name, "IO5", 8);
    b->pins[5].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0};
    b->pins[5].gpio.pinmap = 0;
    b->pins[5].gpio.parent_id = 0;
    b->pins[5].gpio.mux_total = 1;
    b->pins[5].gpio.mux[0].pin = 66;
    b->pins[5].gpio.mux[0].value = 0;
    b->pins[5].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[5].gpio.output_enable = 18;
    b->pins[5].gpio.pullup_enable = 19;
    b->pins[5].pwm.pinmap = 3;
    b->pins[5].pwm.parent_id = 0;
    b->pins[5].pwm.mux_total = 2;
    b->pins[5].pwm.mux[0].pin = 66;
    b->pins[5].pwm.mux[0].value = 1;
    b->pins[5].pwm.mux[1].pin = 18;
    b->pins[5].pwm.mux[1].value = 0;

    strncpy(b->pins[6].name, "IO6", 8);
    b->pins[6].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0};
    b->pins[6].gpio.pinmap = 1;
    b->pins[6].gpio.parent_id = 0;
    b->pins[6].gpio.mux_total = 1;
    b->pins[6].gpio.mux[0].pin = 68;
    b->pins[6].gpio.mux[0].value = 0;
    b->pins[6].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[6].gpio.output_enable = 20;
    b->pins[6].gpio.pullup_enable = 21;
    b->pins[6].pwm.pinmap = 5;
    b->pins[6].pwm.parent_id = 0;
    b->pins[6].pwm.mux_total = 2;
    b->pins[6].pwm.mux[0].pin = 68;
    b->pins[6].pwm.mux[0].value = 1;
    b->pins[6].pwm.mux[1].pin = 20;
    b->pins[6].pwm.mux[1].value = 0;

    strncpy(b->pins[7].name, "IO7", 8);
    b->pins[7].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[7].gpio.pinmap = 38;
    b->pins[7].gpio.parent_id = 0;
    b->pins[7].gpio.mux_total = 0;
    b->pins[7].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[7].gpio.pullup_enable = 39;

    strncpy(b->pins[8].name, "IO8", 8);
    b->pins[8].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[8].gpio.pinmap = 40;
    b->pins[8].gpio.parent_id = 0;
    b->pins[8].gpio.mux_total = 0;
    b->pins[8].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[8].gpio.pullup_enable = 41;

    strncpy(b->pins[9].name, "IO9", 8);
    b->pins[9].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0};
    b->pins[9].gpio.pinmap = 4;
    b->pins[9].gpio.parent_id = 0;
    b->pins[9].gpio.mux_total = 1;
    b->pins[9].gpio.mux[0].pin = 70;
    b->pins[9].gpio.mux[0].value = 0;
    b->pins[9].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[9].gpio.output_enable = 22;
    b->pins[9].gpio.pullup_enable = 23;
    b->pins[9].pwm.pinmap = 7;
    b->pins[9].pwm.parent_id = 0;
    b->pins[9].pwm.mux_total = 2;
    b->pins[9].pwm.mux[0].pin = 70;
    b->pins[9].pwm.mux[0].value = 1;
    b->pins[9].pwm.mux[1].pin = 22;
    b->pins[9].pwm.mux[1].value = 0;

    strncpy(b->pins[10].name, "IO10", 8);
    b->pins[10].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0};
    b->pins[10].gpio.pinmap = 10;
    b->pins[10].gpio.parent_id = 0;
    b->pins[10].gpio.mux_total = 1;
    b->pins[10].gpio.mux[0].pin = 74;
    b->pins[10].gpio.mux[0].value = 0;
    b->pins[10].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[10].gpio.output_enable = 26;
    b->pins[10].gpio.pullup_enable = 27;
    b->pins[10].pwm.pinmap = 11;
    b->pins[10].pwm.parent_id = 0;
    b->pins[10].pwm.mux_total = 2;
    b->pins[10].pwm.mux[0].pin = 74;
    b->pins[10].pwm.mux[0].value = 1;
    b->pins[10].pwm.mux[1].pin = 26;
    b->pins[10].pwm.mux[1].value = 0;

    strncpy(b->pins[11].name, "IO11", 8);
    b->pins[11].capabilites = (mraa_pincapabilities_t) {1,1,1,0,1,0,0};
    b->pins[11].gpio.pinmap = 5;
    b->pins[11].gpio.parent_id = 0;
    b->pins[11].gpio.mux_total = 2;
    b->pins[11].gpio.mux[0].pin = 72;
    b->pins[11].gpio.mux[0].value = 0;
    b->pins[11].gpio.mux[1].pin = 44;
    b->pins[11].gpio.mux[1].value = 0;
    b->pins[11].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[11].gpio.output_enable = 24;
    b->pins[11].gpio.pullup_enable = 25;
    b->pins[11].pwm.pinmap = 9;
    b->pins[11].pwm.parent_id = 0;
    b->pins[11].pwm.mux_total = 3;
    b->pins[11].pwm.mux[0].pin = 72;
    b->pins[11].pwm.mux[0].value = 1;
    b->pins[11].pwm.mux[1].pin = 44;
    b->pins[11].pwm.mux[1].value = 0;
    b->pins[11].pwm.mux[2].pin = 24;
    b->pins[11].pwm.mux[2].value = 0;
    b->pins[11].spi.pinmap = 1;
    b->pins[11].spi.mux_total = 3;
    b->pins[11].spi.mux[0].pin = 72;
    b->pins[11].spi.mux[0].value = 0;
    b->pins[11].spi.mux[1].pin = 44;
    b->pins[11].spi.mux[2].value = 1;
    b->pins[11].pwm.mux[2].pin = 24;
    b->pins[11].pwm.mux[2].value = 0;

    strncpy(b->pins[12].name, "IO12", 8);
    b->pins[12].capabilites = (mraa_pincapabilities_t) {1,1,0,0,1,0,0};
    b->pins[12].gpio.pinmap = 15;
    b->pins[12].gpio.parent_id = 0;
    b->pins[12].gpio.mux_total = 0;
    b->pins[12].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[12].gpio.output_enable = 42;
    b->pins[12].gpio.pullup_enable = 43;
    b->pins[12].spi.pinmap = 1;
    b->pins[12].spi.mux_total = 1;
    b->pins[12].spi.mux[0].pin = 42;
    b->pins[12].spi.mux[0].value = 0;
    // THIS NEEDS TESTING UNSURE IF MOSI WILL BE EXPOSED.

    strncpy(b->pins[13].name, "IO13", 8);
    b->pins[13].capabilites = (mraa_pincapabilities_t) {1,1,0,0,1,0,0};
    b->pins[13].gpio.pinmap = 7;
    b->pins[13].gpio.parent_id = 0;
    b->pins[13].gpio.mux_total = 1;
    b->pins[13].gpio.mux[0].pin = 46;
    b->pins[13].gpio.mux[0].value = 0;
    b->pins[13].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,1,0,1,1};
    b->pins[13].gpio.output_enable = 30;
    b->pins[13].gpio.pullup_enable = 31;
    b->pins[13].spi.pinmap = 1;
    b->pins[13].spi.mux_total = 2;
    b->pins[13].spi.mux[0].pin = 46;
    b->pins[13].spi.mux[0].value = 1;
    b->pins[13].spi.mux[1].pin = 30;
    b->pins[13].spi.mux[1].value = 0;

    //ANALOG
    strncpy(b->pins[14].name, "A0", 8);
    b->pins[14].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,1};
    b->pins[14].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[14].gpio.pullup_enable = 49;
    b->pins[14].aio.pinmap = 0;
    b->pins[14].aio.mux_total = 0;

    strncpy(b->pins[15].name, "A1", 8);
    b->pins[15].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,1};
    b->pins[15].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[15].gpio.pullup_enable = 51;
    b->pins[15].aio.pinmap = 1;
    b->pins[15].aio.mux_total = 0;

    strncpy(b->pins[16].name, "A2", 8);
    b->pins[16].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,1};
    b->pins[16].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[16].gpio.pullup_enable = 53;
    b->pins[16].aio.pinmap = 2;
    b->pins[16].aio.mux_total = 0;

    strncpy(b->pins[17].name, "A3", 8);
    b->pins[17].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,0,1};
    b->pins[17].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[17].gpio.pullup_enable = 55;
    b->pins[17].aio.pinmap = 3;
    b->pins[17].aio.mux_total = 0;

    strncpy(b->pins[18].name, "A4", 8);
    b->pins[18].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,1,1};
    b->pins[18].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[18].gpio.pullup_enable = 57;
    b->pins[18].i2c.pinmap = 1;
    b->pins[18].i2c.mux_total = 1;
    b->pins[18].i2c.mux[0].pin = 60;
    b->pins[18].i2c.mux[0].value = 0;
    b->pins[18].aio.pinmap = 4;
    b->pins[18].aio.mux_total = 2;
    b->pins[18].aio.mux[0].pin = 60;
    b->pins[18].aio.mux[0].value = 1;
    b->pins[18].aio.mux[1].pin = 78;
    b->pins[18].aio.mux[1].value = 0;

    strncpy(b->pins[19].name, "A5", 8);
    b->pins[19].capabilites = (mraa_pincapabilities_t) {1,0,0,0,0,1,1};
    b->pins[19].gpio.complex_cap  = (mraa_pin_cap_complex_t) {1,0,0,1,1};
    b->pins[19].gpio.pullup_enable = 59;
    b->pins[19].i2c.pinmap = 1;
    b->pins[19].i2c.mux_total = 1;
    b->pins[19].i2c.mux[0].pin = 60;
    b->pins[19].i2c.mux[0].value = 0;
    b->pins[19].aio.pinmap = 5;
    b->pins[19].aio.mux_total = 2;
    b->pins[19].aio.mux[0].pin = 60;
    b->pins[19].aio.mux[0].value = 1;
    b->pins[19].aio.mux[1].pin = 79;
    b->pins[19].aio.mux[1].value = 0;

    //BUS DEFINITIONS
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = 18;
    b->i2c_bus[0].scl = 19;

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 1;
    b->spi_bus[0].slave_s = 0;
    b->spi_bus[0].cs = 10;
    b->spi_bus[0].mosi = 11;
    b->spi_bus[0].miso = 12;
    b->spi_bus[0].sclk = 13;

    return b;
}
