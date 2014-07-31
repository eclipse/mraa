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
#include "intel_edison_fab_c.h"

#define SYSFS_PINMODE_PATH "/sys/kernel/debug/gpio_debug/gpio"
#define MAX_SIZE 64
#define MAX_MODE_SIZE 8

typedef struct {
    int sysfs;
    int mode;
} mraa_intel_edision_pindef_t;

typedef struct {
    mraa_intel_edision_pindef_t gpio;
    mraa_intel_edision_pindef_t pwm;
    mraa_intel_edision_pindef_t i2c;
    mraa_intel_edision_pindef_t spi;
    mraa_intel_edision_pindef_t uart;
} mraa_intel_edison_pinmodes_t;

static mraa_gpio_context tristate;

static mraa_intel_edison_pinmodes_t pinmodes[MRAA_INTEL_EDISON_PINCOUNT];
static unsigned int outputen[] = {248,249,250,251,252,253,254,255,256,257,258,259,260,261,232,233,234,235,236,237};
static unsigned int pullup_map[] = {216,217,218,219,220,221,222,223,224,225,226,227,228,229,208,209,210,211,212,213};

static mraa_result_t
mraa_intel_edison_pinmode_change(int sysfs, int mode)
{
    if (mode < 0 ) {
        return MRAA_SUCCESS;
    }
    char buffer[MAX_SIZE];
    snprintf(buffer, MAX_SIZE, SYSFS_PINMODE_PATH "%i/current_pinmux",sysfs);
    int modef = open(buffer, O_WRONLY);
    if (modef == -1) {
        fprintf(stderr, "Failed to open SoC pinmode for opening\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char mode_buf[MAX_MODE_SIZE];
    int length = sprintf(mode_buf, "mode%u",mode);
    if (write(modef, mode_buf, length*sizeof(char)) == -1) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    close(modef);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_gpio_dir_pre(mraa_gpio_context dev, gpio_dir_t dir)
{
    mraa_gpio_write(tristate, 0);
    if (dev->phy_pin >= 0) {
        int pin = dev->phy_pin;

        mraa_gpio_context output_e;
        output_e = mraa_gpio_init_raw(outputen[pin]);
        if (mraa_gpio_dir(output_e, MRAA_GPIO_OUT) != MRAA_SUCCESS)
            return MRAA_ERROR_INVALID_RESOURCE;
        int output_val = 0;
        if (dir == MRAA_GPIO_OUT)
            output_val = 1;
        if (mraa_gpio_write(output_e, output_val) != MRAA_SUCCESS)
            return MRAA_ERROR_INVALID_RESOURCE;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_gpio_dir_post(mraa_gpio_context dev, gpio_dir_t dir)
{
    mraa_gpio_write(tristate, 1);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_gpio_init_post(mraa_gpio_context dev)
{
    if (dev == NULL)
        return MRAA_ERROR_INVALID_RESOURCE;

    int sysfs = pinmodes[dev->phy_pin].gpio.sysfs;
    int mode = pinmodes[dev->phy_pin].gpio.mode;
    mraa_result_t ret = mraa_intel_edison_pinmode_change(sysfs, mode);

    return ret;
}

mraa_result_t
mraa_intel_edison_i2c_init_pre(unsigned int bus)
{
    if(bus != 6) {
        fprintf(stderr, "Edison: You can use that bus, ERR\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    mraa_gpio_write(tristate, 0);
    mraa_gpio_context io18_gpio = mraa_gpio_init_raw(14);
    mraa_gpio_context io19_gpio = mraa_gpio_init_raw(165);
    mraa_gpio_dir(io18_gpio, MRAA_GPIO_IN);
    mraa_gpio_dir(io19_gpio, MRAA_GPIO_IN);
    mraa_gpio_close(io18_gpio);
    mraa_gpio_close(io19_gpio);

    mraa_gpio_context io18_enable = mraa_gpio_init_raw(236);
    mraa_gpio_context io19_enable = mraa_gpio_init_raw(237);
    mraa_gpio_dir(io18_enable, MRAA_GPIO_OUT);
    mraa_gpio_dir(io19_enable, MRAA_GPIO_OUT);
    mraa_gpio_write(io18_enable, 0);
    mraa_gpio_write(io19_enable, 0);
    mraa_gpio_close(io18_enable);
    mraa_gpio_close(io19_enable);

    mraa_gpio_context io18_pullup = mraa_gpio_init_raw(212);
    mraa_gpio_context io19_pullup = mraa_gpio_init_raw(213);
    mraa_gpio_dir(io18_pullup, MRAA_GPIO_IN);
    mraa_gpio_dir(io19_pullup, MRAA_GPIO_IN);
    mraa_gpio_close(io18_pullup);
    mraa_gpio_close(io19_pullup);

    mraa_intel_edison_pinmode_change(28, 1);
    mraa_intel_edison_pinmode_change(27, 1);

    mraa_gpio_write(tristate, 1);
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_intel_edison_misc_spi()
{
    mraa_gpio_write(tristate, 0);

    mraa_gpio_context io10_p1 = mraa_gpio_init_raw(263);
    mraa_gpio_context io10_p2 = mraa_gpio_init_raw(240);
    mraa_gpio_context io11_p1 = mraa_gpio_init_raw(262);
    mraa_gpio_context io11_p2 = mraa_gpio_init_raw(241);
    mraa_gpio_context io12_p1 = mraa_gpio_init_raw(242);
    mraa_gpio_context io13_p1 = mraa_gpio_init_raw(243);
    mraa_gpio_dir(io10_p1, MRAA_GPIO_OUT);
    mraa_gpio_dir(io10_p2, MRAA_GPIO_OUT);
    mraa_gpio_dir(io11_p1, MRAA_GPIO_OUT);
    mraa_gpio_dir(io11_p2, MRAA_GPIO_OUT);
    mraa_gpio_dir(io12_p1, MRAA_GPIO_OUT);
    mraa_gpio_dir(io13_p1, MRAA_GPIO_OUT);

    mraa_gpio_write(io10_p1, 1);
    mraa_gpio_write(io10_p2, 0);
    mraa_gpio_write(io11_p1, 1);
    mraa_gpio_write(io11_p2, 0);
    mraa_gpio_write(io12_p1, 0);
    mraa_gpio_write(io13_p1, 0);

    mraa_gpio_close(io10_p1);
    mraa_gpio_close(io10_p2);
    mraa_gpio_close(io11_p1);
    mraa_gpio_close(io11_p2);
    mraa_gpio_close(io12_p1);
    mraa_gpio_close(io13_p1);

    mraa_intel_edison_pinmode_change(111, 1);
    mraa_intel_edison_pinmode_change(115, 1);
    mraa_intel_edison_pinmode_change(114, 1);
    mraa_intel_edison_pinmode_change(109, 1);
    mraa_gpio_write(tristate, 1);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_aio_get_fp(mraa_aio_context dev)
{
    char file_path[64]= "";

    snprintf(file_path, 64, "/sys/bus/iio/devices/iio:device1/in_voltage%d_raw",
        dev->channel );

    dev->adc_in_fp = open(file_path, O_RDONLY);
    if (dev->adc_in_fp == -1) {
    fprintf(stderr, "Failed to open Analog input raw file %s for "
        "reading!\n", file_path); return( MRAA_ERROR_INVALID_RESOURCE);
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_aio_init_pre(unsigned int aio)
{
    if (aio > plat->aio_count) {
        fprintf(stderr, "Invalid analog input channel\n");
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    int pin = 14 + aio;
    mraa_gpio_context output_e;
    output_e = mraa_gpio_init_raw(outputen[pin]);
    if (mraa_gpio_dir(output_e, MRAA_GPIO_OUT) != MRAA_SUCCESS)
        return MRAA_ERROR_INVALID_RESOURCE;
    if (mraa_gpio_write(output_e, 0) != MRAA_SUCCESS)
        return MRAA_ERROR_INVALID_RESOURCE;
    mraa_gpio_close(output_e);

    mraa_gpio_context pullup_pin;
    pullup_pin = mraa_gpio_init_raw(pullup_map[pin]);
    if (mraa_gpio_dir(pullup_pin, MRAA_GPIO_IN) != MRAA_SUCCESS)
        return MRAA_ERROR_INVALID_RESOURCE;
    mraa_gpio_close(pullup_pin);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_aio_init_post(mraa_aio_context dev)
{
    mraa_gpio_write(tristate, 1);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_pwm_init_pre(int pin)
{
    if (pin < 0 || pin > 19)
        return MRAA_ERROR_INVALID_RESOURCE;

    if (!plat->pins[pin].capabilites.pwm)
        return MRAA_ERROR_INVALID_RESOURCE;

    mraa_gpio_context output_e;
    output_e = mraa_gpio_init_raw(outputen[pin]);
    if (mraa_gpio_dir(output_e, MRAA_GPIO_OUT) != MRAA_SUCCESS)
        return MRAA_ERROR_INVALID_RESOURCE;
    if (mraa_gpio_write(output_e, 1) != MRAA_SUCCESS)
        return MRAA_ERROR_INVALID_RESOURCE;
    mraa_gpio_close(output_e);

    mraa_gpio_context pullup_pin;
    pullup_pin = mraa_gpio_init_raw(pullup_map[pin]);
    if (mraa_gpio_dir(pullup_pin, MRAA_GPIO_IN) != MRAA_SUCCESS)
        return MRAA_ERROR_INVALID_RESOURCE;
    mraa_gpio_close(pullup_pin);
    mraa_intel_edison_pinmode_change(plat->pins[pin].gpio.pinmap, 1);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_pwm_init_post(mraa_pwm_context pwm)
{
    mraa_gpio_write(tristate, 1);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_spi_init_pre(int bus)
{
    mraa_gpio_write(tristate, 0);

    mraa_gpio_context io10_out = mraa_gpio_init_raw(258);
    mraa_gpio_context io11_out = mraa_gpio_init_raw(259);
    mraa_gpio_context io12_out = mraa_gpio_init_raw(260);
    mraa_gpio_context io13_out = mraa_gpio_init_raw(261);
    mraa_gpio_dir(io10_out, MRAA_GPIO_OUT);
    mraa_gpio_dir(io11_out, MRAA_GPIO_OUT);
    mraa_gpio_dir(io12_out, MRAA_GPIO_OUT);
    mraa_gpio_dir(io13_out, MRAA_GPIO_OUT);

    mraa_gpio_write(io10_out, 1);
    mraa_gpio_write(io11_out, 1);
    mraa_gpio_write(io12_out, 0);
    mraa_gpio_write(io13_out, 1);

    mraa_gpio_close(io10_out);
    mraa_gpio_close(io11_out);
    mraa_gpio_close(io12_out);
    mraa_gpio_close(io13_out);

    mraa_gpio_context io10_pull = mraa_gpio_init_raw(226);
    mraa_gpio_context io11_pull = mraa_gpio_init_raw(227);
    mraa_gpio_context io12_pull = mraa_gpio_init_raw(228);
    mraa_gpio_context io13_pull = mraa_gpio_init_raw(229);

    mraa_gpio_dir(io10_pull, MRAA_GPIO_IN);
    mraa_gpio_dir(io11_pull, MRAA_GPIO_IN);
    mraa_gpio_dir(io12_pull, MRAA_GPIO_IN);
    mraa_gpio_dir(io13_pull, MRAA_GPIO_IN);

    mraa_gpio_close(io10_pull);
    mraa_gpio_close(io11_pull);
    mraa_gpio_close(io12_pull);
    mraa_gpio_close(io13_pull);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_intel_edison_spi_init_post(mraa_spi_context spi)
{
    mraa_gpio_write(tristate, 1);
    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_intel_edison_fab_c()
{
    mraa_board_t* b = (mraa_board_t*) malloc(sizeof(mraa_board_t));
    if (b == NULL)
        return NULL;

    b->phy_pin_count = 20;
    b->gpio_count = 14;
    b->aio_count = 6;

    advance_func->gpio_dir_pre = &mraa_intel_edison_gpio_dir_pre;
    advance_func->gpio_init_post = &mraa_intel_edison_gpio_init_post;
    advance_func->gpio_dir_post = &mraa_intel_edison_gpio_dir_post;
    advance_func->i2c_init_pre = &mraa_intel_edison_i2c_init_pre;
    advance_func->aio_get_valid_fp = &mraa_intel_edison_aio_get_fp;
    advance_func->aio_init_pre = &mraa_intel_edison_aio_init_pre;
    advance_func->aio_init_post = &mraa_intel_edison_aio_init_post;
    advance_func->pwm_init_pre = &mraa_intel_edison_pwm_init_pre;
    advance_func->pwm_init_post = &mraa_intel_edison_pwm_init_post;
    advance_func->spi_init_pre = &mraa_intel_edison_spi_init_pre;
    advance_func->spi_init_post = &mraa_intel_edison_spi_init_post;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t)*MRAA_INTEL_EDISON_PINCOUNT);

    tristate = mraa_gpio_init_raw(214);
    if (tristate == NULL) {
        fprintf(stderr, "Intel Edison Failed to initialise Arduino board TriState,\
                         check i2c devices! FATAL\n");
        return NULL;
    }
    mraa_gpio_dir(tristate, MRAA_GPIO_OUT);
    mraa_intel_edison_misc_spi();

    strncpy(b->pins[0].name, "IO0", 8);
    b->pins[0].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[0].gpio.pinmap = 130;
    b->pins[0].gpio.parent_id = 0;
    b->pins[0].gpio.mux_total = 0;

    strncpy(b->pins[1].name, "IO1", 8);
    b->pins[1].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[1].gpio.pinmap = 131;
    b->pins[1].gpio.parent_id = 0;
    b->pins[1].gpio.mux_total = 0;

    strncpy(b->pins[2].name, "IO2", 8);
    b->pins[2].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[2].gpio.pinmap = 128;
    b->pins[2].gpio.parent_id = 0;
    b->pins[2].gpio.mux_total = 0;

    strncpy(b->pins[3].name, "IO3", 8);
    b->pins[3].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0};
    b->pins[3].gpio.pinmap = 12;
    b->pins[3].gpio.parent_id = 0;
    b->pins[3].gpio.mux_total = 0;
    b->pins[3].pwm.pinmap = 0;
    b->pins[3].pwm.parent_id = 0;
    b->pins[3].pwm.mux_total = 0;

    strncpy(b->pins[4].name, "IO4", 8);
    b->pins[4].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[4].gpio.pinmap = 129;
    b->pins[4].gpio.parent_id = 0;
    b->pins[4].gpio.mux_total = 0;

    strncpy(b->pins[5].name, "IO5", 8);
    b->pins[5].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0,0};
    b->pins[5].gpio.pinmap = 13;
    b->pins[5].gpio.parent_id = 0;
    b->pins[5].gpio.mux_total = 0;
    b->pins[5].pwm.pinmap = 1;
    b->pins[5].pwm.parent_id = 0;
    b->pins[5].pwm.mux_total = 0;

    strncpy(b->pins[6].name, "IO6", 8);
    b->pins[6].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0,0};
    b->pins[6].gpio.pinmap = 182;
    b->pins[6].gpio.parent_id = 0;
    b->pins[6].gpio.mux_total = 0;
    b->pins[6].pwm.pinmap = 2;
    b->pins[6].pwm.parent_id = 0;
    b->pins[6].pwm.mux_total = 0;

    strncpy(b->pins[7].name, "IO7", 8);
    b->pins[7].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[7].gpio.pinmap = 48;
    b->pins[7].gpio.parent_id = 0;
    b->pins[7].gpio.mux_total = 0;

    strncpy(b->pins[8].name, "IO8", 8);
    b->pins[8].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,0};
    b->pins[8].gpio.pinmap = 49;
    b->pins[8].gpio.parent_id = 0;
    b->pins[8].gpio.mux_total = 0;

    strncpy(b->pins[9].name, "IO9", 8);
    b->pins[9].capabilites = (mraa_pincapabilities_t) {1,1,1,0,0,0,0,0};
    b->pins[9].gpio.pinmap = 183;
    b->pins[9].gpio.parent_id = 0;
    b->pins[9].gpio.mux_total = 0;
    b->pins[9].pwm.pinmap = 3;
    b->pins[9].pwm.parent_id = 0;
    b->pins[9].pwm.mux_total = 0;

    strncpy(b->pins[10].name, "IO10", 8);
    b->pins[10].capabilites = (mraa_pincapabilities_t) {1,1,0,0,1,0,0,0};
    b->pins[10].gpio.pinmap = 41;
    b->pins[10].gpio.parent_id = 0;
    b->pins[10].gpio.mux_total = 2;
    b->pins[10].gpio.mux[0].pin = 263;
    b->pins[10].gpio.mux[0].value = 1;
    b->pins[10].gpio.mux[1].pin = 240;
    b->pins[10].gpio.mux[1].value = 0;
    b->pins[10].spi.pinmap = 5;
    b->pins[10].spi.mux_total = 2;
    b->pins[10].spi.mux[0].pin = 263;
    b->pins[10].spi.mux[0].value = 1;
    b->pins[10].spi.mux[1].pin = 240;
    b->pins[10].spi.mux[1].value = 1;

    strncpy(b->pins[11].name, "IO11", 8);
    b->pins[11].capabilites = (mraa_pincapabilities_t) {1,1,0,0,1,0,0,0};
    b->pins[11].gpio.pinmap = 43;
    b->pins[11].gpio.parent_id = 0;
    b->pins[11].gpio.mux_total = 2;
    b->pins[11].gpio.mux[0].pin = 262;
    b->pins[11].gpio.mux[0].value = 1;
    b->pins[11].gpio.mux[1].pin = 241;
    b->pins[11].gpio.mux[1].value = 0;
    b->pins[11].spi.pinmap = 5;
    b->pins[11].spi.mux_total = 2;
    b->pins[11].spi.mux[0].pin = 262;
    b->pins[11].spi.mux[0].value = 1;
    b->pins[11].spi.mux[1].pin = 241;
    b->pins[11].spi.mux[1].value = 1;

    strncpy(b->pins[12].name, "IO12", 8);
    b->pins[12].capabilites = (mraa_pincapabilities_t) {1,1,0,0,1,0,0,0};
    b->pins[12].gpio.pinmap = 42;
    b->pins[12].gpio.parent_id = 0;
    b->pins[12].gpio.mux_total = 1;
    b->pins[12].gpio.mux[0].pin = 242;
    b->pins[12].gpio.mux[0].value = 0;
    b->pins[12].spi.pinmap = 5;
    b->pins[12].spi.mux_total = 1;
    b->pins[12].spi.mux[0].pin = 242;
    b->pins[12].spi.mux[0].value = 1;

    strncpy(b->pins[13].name, "IO13", 8);
    b->pins[13].capabilites = (mraa_pincapabilities_t) {1,1,0,0,1,0,0,0};
    b->pins[13].gpio.pinmap = 40;
    b->pins[13].gpio.parent_id = 0;
    b->pins[13].gpio.mux_total = 1;
    b->pins[13].gpio.mux[0].pin = 243;
    b->pins[13].gpio.mux[0].value = 0;
    b->pins[13].spi.pinmap = 5;
    b->pins[13].spi.mux_total = 1;
    b->pins[13].spi.mux[0].pin = 243;
    b->pins[13].spi.mux[0].value = 1;

    strncpy(b->pins[14].name, "A0", 8);
    b->pins[14].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,1,0};
    b->pins[14].aio.pinmap = 0;
    b->pins[14].aio.mux_total = 1;
    b->pins[14].aio.mux[0].pin = 200;
    b->pins[14].aio.mux[0].value = 1;
    b->pins[14].gpio.pinmap = 44;
    b->pins[14].gpio.mux_total = 1;
    b->pins[14].gpio.mux[0].pin = 200;
    b->pins[14].gpio.mux[0].value = 0;

    strncpy(b->pins[15].name, "A1", 8);
    b->pins[15].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,1,0};
    b->pins[15].aio.pinmap = 1;
    b->pins[15].aio.mux_total = 1;
    b->pins[15].aio.mux[0].pin = 201;
    b->pins[15].aio.mux[0].value = 1;
    b->pins[15].gpio.pinmap = 45;
    b->pins[15].gpio.mux_total = 1;
    b->pins[15].gpio.mux[0].pin = 201;
    b->pins[15].gpio.mux[0].value = 0;

    strncpy(b->pins[16].name, "A2", 8);
    b->pins[16].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,1,0};
    b->pins[16].aio.pinmap = 2;
    b->pins[16].aio.mux_total = 1;
    b->pins[16].aio.mux[0].pin = 202;
    b->pins[16].aio.mux[0].value = 1;
    b->pins[16].gpio.pinmap = 46;
    b->pins[16].gpio.mux_total = 1;
    b->pins[16].gpio.mux[0].pin = 202;
    b->pins[16].gpio.mux[0].value = 0;

    strncpy(b->pins[17].name, "A3", 8);
    b->pins[17].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,0,1,0};
    b->pins[17].aio.pinmap = 3;
    b->pins[17].aio.mux_total = 1;
    b->pins[17].aio.mux[0].pin = 203;
    b->pins[17].aio.mux[0].value = 1;
    b->pins[17].gpio.pinmap = 47;
    b->pins[17].gpio.mux_total = 1;
    b->pins[17].gpio.mux[0].pin = 203;
    b->pins[17].gpio.mux[0].value = 0;

    strncpy(b->pins[18].name, "A4", 8);
    b->pins[18].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,1,1,0};
    b->pins[18].i2c.pinmap = 1;
    b->pins[18].i2c.mux_total = 1;
    b->pins[18].i2c.mux[0].pin = 204;
    b->pins[18].i2c.mux[0].value = 0;
    b->pins[18].aio.pinmap = 4;
    b->pins[18].aio.mux_total = 1;
    b->pins[18].aio.mux[0].pin = 204;
    b->pins[18].aio.mux[0].value = 1;
    b->pins[18].gpio.pinmap = 14;
    b->pins[18].gpio.mux_total = 1;
    b->pins[18].gpio.mux[0].pin = 204;
    b->pins[18].gpio.mux[0].value = 0;

    strncpy(b->pins[19].name, "A5", 8);
    b->pins[19].capabilites = (mraa_pincapabilities_t) {1,1,0,0,0,1,1,0};
    b->pins[19].i2c.pinmap = 1;
    b->pins[19].i2c.mux_total = 1;
    b->pins[19].i2c.mux[0].pin = 205;
    b->pins[19].i2c.mux[0].value = 0;
    b->pins[19].aio.pinmap = 5;
    b->pins[19].aio.mux_total = 1;
    b->pins[19].aio.mux[0].pin = 205;
    b->pins[19].aio.mux[0].value = 1;
    b->pins[19].gpio.pinmap = 165;
    b->pins[19].gpio.mux_total = 1;
    b->pins[19].gpio.mux[0].pin = 205;
    b->pins[19].gpio.mux[0].value = 0;

    //BUS DEFINITIONS
    b->i2c_bus_count = 9;
    b->def_i2c_bus = 6;
    int ici;
    for (ici = 0; ici < 9; ici++) {
        b->i2c_bus[ici].bus_id = -1;
    }
    b->i2c_bus[6].bus_id = 6;
    b->i2c_bus[6].sda = 18;
    b->i2c_bus[6].scl = 19;

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 5;
    b->spi_bus[0].slave_s = 1;
    b->spi_bus[0].cs = 10;
    b->spi_bus[0].mosi = 11;
    b->spi_bus[0].miso = 12;
    b->spi_bus[0].sclk = 13;

    int il;

    for(il =0; il < MRAA_INTEL_EDISON_PINCOUNT; il++) {
        pinmodes[il].gpio.sysfs = -1;
        pinmodes[il].gpio.mode = -1;
        pinmodes[il].pwm.sysfs = -1;
        pinmodes[il].pwm.mode = -1;
        pinmodes[il].i2c.sysfs = -1;
        pinmodes[il].i2c.mode = -1;
        pinmodes[il].spi.sysfs = -1;
        pinmodes[il].spi.mode = -1;
        pinmodes[il].uart.sysfs = -1;
        pinmodes[il].uart.mode = -1;
    }
    pinmodes[0].gpio.sysfs = 130;
    pinmodes[0].gpio.mode = 0;
    pinmodes[0].uart.sysfs = 130;
    pinmodes[0].uart.mode = 1;
    pinmodes[1].gpio.sysfs = 131;
    pinmodes[1].gpio.mode = 0;
    pinmodes[1].uart.sysfs = 131;
    pinmodes[1].uart.mode = 1;
    pinmodes[2].gpio.sysfs = 128;
    pinmodes[2].gpio.mode = 0;
    pinmodes[2].uart.sysfs = 128;
    pinmodes[2].uart.mode = 1;
    pinmodes[3].gpio.sysfs = 12;
    pinmodes[3].gpio.mode = 0;
    pinmodes[3].pwm.sysfs = 12;
    pinmodes[3].pwm.mode = 1;

    pinmodes[4].gpio.sysfs = 129;
    pinmodes[4].gpio.mode = 0;
    pinmodes[4].uart.sysfs = 129;
    pinmodes[4].uart.mode = 1;
    pinmodes[5].gpio.sysfs = 13;
    pinmodes[5].gpio.mode = 0;
    pinmodes[5].pwm.sysfs = 13;
    pinmodes[5].pwm.mode = 1;
    pinmodes[6].gpio.sysfs = 182;
    pinmodes[6].gpio.mode = 0;
    pinmodes[6].pwm.sysfs = 182;
    pinmodes[6].pwm.mode = 1;

    //7 and 8 are provided by something on i2c, very simplepinmodes[3].gpio.sysfs = 12;
    pinmodes[9].gpio.sysfs = 183;
    pinmodes[9].gpio.mode = 0;
    pinmodes[9].pwm.sysfs = 183;
    pinmodes[9].pwm.mode = 1;

    pinmodes[10].gpio.sysfs = 41;
    pinmodes[10].gpio.mode = 0;
    pinmodes[10].spi.sysfs = 111; // Different pin provides, switched at mux level.
    pinmodes[10].spi.mode = 1;

    pinmodes[11].gpio.sysfs = 43;
    pinmodes[11].gpio.mode = 0;
    pinmodes[11].spi.sysfs = 115; // Different pin provides, switched at mux level.
    pinmodes[11].spi.mode = 1;

    pinmodes[12].gpio.sysfs = 42;
    pinmodes[12].gpio.mode = 0;
    pinmodes[12].spi.sysfs = 114; // Different pin provides, switched at mux level.
    pinmodes[12].spi.mode = 1;

    pinmodes[13].gpio.sysfs = 40;
    pinmodes[13].gpio.mode = 0;
    pinmodes[13].spi.sysfs = 109; // Different pin provides, switched at mux level.
    pinmodes[13].spi.mode = 1;
    //Everything else but A4 A5 LEAVE
    pinmodes[18].gpio.sysfs = 14;
    pinmodes[18].gpio.mode = 0;
    pinmodes[18].i2c.sysfs = 28;
    pinmodes[18].i2c.mode = 1;

    pinmodes[19].gpio.sysfs = 165;
    pinmodes[19].gpio.mode = 0;
    pinmodes[19].i2c.sysfs = 27;
    pinmodes[19].i2c.mode = 1;

    return b;
}
