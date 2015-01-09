/*
 * Author: Henry Bruce <henry.bruce@intel.com>
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
#include <sys/utsname.h>

#include "common.h"
#include "x86/intel_minnow_max.h"

#define PLATFORM_NAME "MinnowBoard MAX"
#define I2C_BUS_COUNT 10
#define I2C_BUS_DEFAULT 7

mraa_result_t
mraa_set_pininfo(mraa_board_t* board, int mraa_index, char *name, mraa_pincapabilities_t caps, int sysfs_pin)
{
    if (mraa_index < board->phy_pin_count) {
        mraa_pininfo_t* pin_info = &board->pins[mraa_index];
        strncpy(pin_info->name, name, 7);
        pin_info->capabilites = caps;
        if (caps.gpio)
            pin_info->gpio.pinmap = sysfs_pin;
        if (caps.i2c) {
            pin_info->i2c.pinmap = 1;
            pin_info->i2c.mux_total = 0;
        }
        if (caps.pwm) {
            int controller = 0;
            if (strncmp(name, "PWM", 3) == 0 && strlen(name) > 3 && isdigit(name[3]))
                controller = name[3] - '0';
            pin_info->pwm.parent_id = controller;
            pin_info->pwm.pinmap = 0;
            pin_info->pwm.mux_total = 0;
        }
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_result_t
mraa_get_pin_index(mraa_board_t* board, char *name, int* pin_index) {
    int i;
    for (i = 0; i < board->phy_pin_count; ++i) {
        if (strcmp(name, board->pins[i].name) == 0) {
            *pin_index = i;
            return MRAA_SUCCESS;
        }
    }
    return MRAA_ERROR_INVALID_RESOURCE;
}

mraa_board_t*
mraa_intel_minnow_max()
{
    mraa_board_t* b = (mraa_board_t*) malloc(sizeof(mraa_board_t));

    struct utsname running_uname;
    int uname_major, uname_minor, max_pins[27];

    if (b == NULL) {
        return NULL;
    }

    b->platform_name_length = strlen(PLATFORM_NAME) + 1;
    b->platform_name = (char*) malloc(sizeof(char) * b->platform_name_length);
    if (b->platform_name == NULL) {
        goto error;
    }
    strncpy(b->platform_name, PLATFORM_NAME, b->platform_name_length);

    b->phy_pin_count = MRAA_INTEL_MINNOW_MAX_PINCOUNT;
    //b->gpio_count = 14;
    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t)*MRAA_INTEL_MINNOW_MAX_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    if (uname(&running_uname) != 0) {
        goto error;
    }

    sscanf(running_uname.release, "%d.%d", &uname_major, &uname_minor);

    /* if we are on Linux 3.17 or lower they use a 256 max and number the GPIOs down
     * if we are on 3.18 or higher (ea584595fc85e65796335033dfca25ed655cd0ed)  (for now)
     * they start at 512 and number down, at some point this is going to change again when
     * GPIO moves to a radix.
     */
    if( uname_major <= 3 && uname_minor <= 17 ){
	max_pins[0]  =  -1; // INVALID, doesn't exist
	max_pins[1]  =  -1; // GND
	max_pins[2]  =  -1; // GND
	max_pins[3]  =  -1; // 5v
	max_pins[4]  =  -1; // 3.3v
	max_pins[5]  = 220; // SPI_CS
	max_pins[6]  = 225; // UART1TX
	max_pins[7]  = 221; // SPIMISO
	max_pins[8]  = 224; // UART1RX
	max_pins[9]  = 222; // SPIMOSI
	max_pins[10] = 227; // UART1CT
	max_pins[11] = 223; // SPI_CLK
	max_pins[12] = 226; // UART1RT
	max_pins[13] = 243; // I2C_SCL
	max_pins[14] = 216; // I2S_CLK
	max_pins[15] = 242; // I2C_SDA
	max_pins[16] = 217; // I2S_FRM
	max_pins[17] = 229; // UART2TX
	max_pins[18] = 219; // I2S_DO
	max_pins[19] = 228; // UART2RX
	max_pins[20] = 218; // I2S_DI
	max_pins[21] =  82; // S5_0
	max_pins[22] = 248; // PWM0
	max_pins[23] =  83; // S5_1
	max_pins[24] = 249; // PWM1
	max_pins[25] =  84; // S5_4
	max_pins[26] = 208; // IBL8254
    } else {
	// This is mainly for +3.18
	max_pins[0]  =  -1; // INVALID, doesn't exist
	max_pins[1]  =  -1; // GND
	max_pins[2]  =  -1; // GND
	max_pins[3]  =  -1; // 5v
	max_pins[4]  =  -1; // 3.3v
	max_pins[5]  = 476; // SPI_CS
	max_pins[6]  = 481; // UART1TX
	max_pins[7]  = 477; // SPIMISO
	max_pins[8]  = 480; // UART1RX
	max_pins[9]  = 478; // SPIMOSI
	max_pins[10] = 483; // UART1CT
	max_pins[11] = 479; // SPI_CLK
	max_pins[12] = 484; // UART1RT
	max_pins[13] = 499; // I2C_SCL
	max_pins[14] = 472; // I2S_CLK
	max_pins[15] = 498; // I2C_SDA
	max_pins[16] = 473; // I2S_FRM
	max_pins[17] = 485; // UART2TX
	max_pins[18] = 475; // I2S_DO
	max_pins[19] = 484; // UART2RX
	max_pins[20] = 474; // I2S_DI
	max_pins[21] = 338; // S5_0
	max_pins[22] = 504; // PWM0
	max_pins[23] = 339; // S5_1
	max_pins[24] = 505; // PWM1
	max_pins[25] = 340; // S5_4
	max_pins[26] = 464; // IBL8254
    }

    mraa_set_pininfo(b,  0, "INVALID",  (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, max_pins[0]);
    mraa_set_pininfo(b,  1, "GND",      (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, max_pins[1]);
    mraa_set_pininfo(b,  2, "GND",      (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, max_pins[2]);
    mraa_set_pininfo(b,  3, "5v",       (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, max_pins[3]);
    mraa_set_pininfo(b,  4, "3.3v",     (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, max_pins[4]);
    mraa_set_pininfo(b,  5, "SPI_CS",   (mraa_pincapabilities_t){1,0,0,0,1,0,0,0}, max_pins[5]);
    mraa_set_pininfo(b,  6, "UART1TX",  (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, max_pins[6]);
    mraa_set_pininfo(b,  7, "SPIMISO",  (mraa_pincapabilities_t){1,0,0,0,1,0,0,0}, max_pins[7]);
    mraa_set_pininfo(b,  8, "UART1RX",  (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, max_pins[8]);
    mraa_set_pininfo(b,  9, "SPIMOSI",  (mraa_pincapabilities_t){1,0,0,0,1,0,0,0}, max_pins[9]);
    mraa_set_pininfo(b, 10, "UART1CT",  (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[10]);
    mraa_set_pininfo(b, 11, "SPI_CLK",  (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, max_pins[11]);
    mraa_set_pininfo(b, 12, "UART1RT",  (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[12]);
    mraa_set_pininfo(b, 13, "I2C_SCL",  (mraa_pincapabilities_t){1,0,0,0,0,1,0,0}, max_pins[13]);
    mraa_set_pininfo(b, 14, "I2S_CLK",  (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[14]);
    mraa_set_pininfo(b, 15, "I2C_SDA",  (mraa_pincapabilities_t){1,0,0,0,0,1,0,0}, max_pins[15]);
    mraa_set_pininfo(b, 16, "I2S_FRM",  (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[16]);
    mraa_set_pininfo(b, 17, "UART2TX",  (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, max_pins[17]);
    mraa_set_pininfo(b, 18, "I2S_DO",   (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[18]);
    mraa_set_pininfo(b, 19, "UART2RX",  (mraa_pincapabilities_t){1,0,0,0,0,0,0,1}, max_pins[19]);
    mraa_set_pininfo(b, 20, "I2S_DI",   (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[20]);
    mraa_set_pininfo(b, 21, "S5_0",     (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[21]);
    mraa_set_pininfo(b, 22, "PWM0",     (mraa_pincapabilities_t){1,0,1,0,0,0,0,0}, max_pins[22]); // Assume BIOS configured for PWM
    mraa_set_pininfo(b, 23, "S5_1",     (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[23]);
    mraa_set_pininfo(b, 24, "PWM1",     (mraa_pincapabilities_t){1,0,1,0,0,0,0,0}, max_pins[24]); // Assume BIOS configured for PWM
    mraa_set_pininfo(b, 25, "S5_4",     (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[25]);
    mraa_set_pininfo(b, 26, "IBL8254",  (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, max_pins[26]);

    // Set number of i2c adaptors
    // Got this from running 'i2cdetect -l'
    b->i2c_bus_count = I2C_BUS_COUNT;

    // Disable all i2c adaptors
    int ici;
    for (ici = 0; ici < b->i2c_bus_count; ici++) {
        b->i2c_bus[ici].bus_id = -1;
    }

    // Configure i2c adaptor #7 and make it the default
    int pin_index_sda, pin_index_scl;
    if (mraa_get_pin_index(b, "I2C_SDA", &pin_index_sda) == MRAA_SUCCESS &&
        mraa_get_pin_index(b, "I2C_SCL", &pin_index_scl) == MRAA_SUCCESS) {
        b->def_i2c_bus = I2C_BUS_DEFAULT;
        b->i2c_bus[b->def_i2c_bus].bus_id = b->def_i2c_bus;
        b->i2c_bus[b->def_i2c_bus].sda = pin_index_sda;
        b->i2c_bus[b->def_i2c_bus].scl = pin_index_scl;
    }

    // Configure PWM
    b->pwm_default_period = 500;
    b->pwm_max_period = 1000000000;
    b->pwm_min_period = 1;

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[0].slave_s = 0;
    b->spi_bus[0].cs = 5;
    b->spi_bus[0].mosi = 9;
    b->spi_bus[0].miso = 7;
    b->spi_bus[0].sclk = 11;

    return b;
error:
    syslog(LOG_CRIT, "minnowmax: Platform failed to initialise");
    free(b);
    return NULL;
}
