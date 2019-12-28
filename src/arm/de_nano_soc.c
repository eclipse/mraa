/*
 * Author: Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2017 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <mraa/common.h>

#include "common.h"
#include "arm/de_nano_soc.h"

#define PLATFORM_NAME "DE0/DE10-Nano-SoC"
#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define DEBUGFS_PINMODE_PATH "/sys/kernel/debug/gpio"

#define FPGA_REGION_BASE 0xFF200000
#define FPGA_REGION_SIZE 0x00200000

#define MMAP_PATH "/dev/mem"


// MMAP
static uint8_t* mmap_reg = NULL;
static int mmap_fd = 0;
static int mmap_size = FPGA_REGION_SIZE;
static unsigned int mmap_count = 0;


// MMAP stubbed functions
mraa_result_t
mraa_de_nano_soc_spi_init_pre(int index)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_de_nano_soc_i2c_init_pre(unsigned int bus)
{
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_de_nano_soc_mmap_write(mraa_gpio_context dev, int value)
{
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_de_nano_soc_mmap_unsetup()
{
    if (mmap_reg == NULL) {
        syslog(LOG_ERR, "de_nano_soc mmap: null register cant unsetup");
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    munmap(mmap_reg, mmap_size);
    mmap_reg = NULL;
    if (close(mmap_fd) != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    return MRAA_SUCCESS;
}

int
mraa_de_nano_soc_mmap_read(mraa_gpio_context dev)
{
    return 0;
}

mraa_result_t
mraa_de_nano_soc_mmap_setup(mraa_gpio_context dev, mraa_boolean_t en)
{

    if (dev == NULL) {
        syslog(LOG_ERR, "de_nano_soc mmap: context not valid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (en == 0) {
        if (dev->mmap_write == NULL && dev->mmap_read == NULL) {
            syslog(LOG_ERR, "de_nano_soc mmap: can't disable disabled mmap gpio");
            return MRAA_ERROR_INVALID_PARAMETER;
        }
        dev->mmap_write = NULL;
        dev->mmap_read = NULL;
        mmap_count--;
        if (mmap_count == 0) {
            return mraa_de_nano_soc_mmap_unsetup();
        }
        return MRAA_SUCCESS;
    }

    if (dev->mmap_write != NULL && dev->mmap_read != NULL) {
        syslog(LOG_ERR, "de_nano_soc mmap: can't enable enabled mmap gpio");
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    // Might need to make some elements of this thread safe.
    // For example only allow one thread to enter the following block
    // to prevent mmap'ing twice.
    if (mmap_reg == NULL) {
        if ((mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
            syslog(LOG_ERR, "de_nano_soc map: unable to open resource0 file");
            return MRAA_ERROR_INVALID_HANDLE;
        }


        mmap_reg = (uint8_t*) mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, mmap_fd, FPGA_REGION_BASE);
        if (mmap_reg == MAP_FAILED) {
            syslog(LOG_ERR, "de_nano_soc mmap: failed to mmap");
            mmap_reg = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    dev->mmap_write = &mraa_de_nano_soc_mmap_write;
    dev->mmap_read = &mraa_de_nano_soc_mmap_read;
    mmap_count++;

    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_de_nano_soc()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = MRAA_DE_NANO_SOC_PINCOUNT;
    b->gpio_count = 96; // update as needed when adding ADC pins
    b->aio_count = 8;
    //b->pwm_default_period = 5000;
    //b->pwm_max_period = 218453;
    //b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b);
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        free(b);
        return NULL;
    }

    int pos = 0;

    strncpy(b->pins[pos].name, "D0/RX", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    //b->pins[pos].gpio.pinmap = 0;
    //b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D1/TX", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    //b->pins[pos].gpio.pinmap = 0;
    //b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D2", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 203;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D3", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 204;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D4", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 205;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D5", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 206;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D6", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 207;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D7", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 208;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D8", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 209;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "D9", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 210;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    // TODO: add rest of the Arduino header
    while (pos < 32) {
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        pos++;
    }

    // FPGA GPIO-0/JP1
    int jp = 1;
    int os_index = 171;
    while (pos < 52) {
        if (jp == 11) {
            strncpy(b->pins[pos].name, "5V", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else if (jp == 12) {
            strncpy(b->pins[pos].name, "GND", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else {
            snprintf(b->pins[pos].name, 8, "JP1-%d", jp);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
            b->pins[pos].gpio.pinmap = os_index;
            b->pins[pos].gpio.mux_total = 0;
            os_index++;
        }
        pos++;
        jp++;
    }

    os_index = 139;
    while (pos < 72) {
        if (jp == 29) {
            strncpy(b->pins[pos].name, "3V3", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else if (jp == 30) {
            strncpy(b->pins[pos].name, "GND", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else {
            snprintf(b->pins[pos].name, 8, "JP1-%d", jp);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
            b->pins[pos].gpio.pinmap = os_index;
            b->pins[pos].gpio.mux_total = 0;
            os_index++;
        }
        pos++;
        jp++;
    }

    // FPGA GPIO-1/JP7
    jp = 1;
    os_index = 107;
    while (pos < 92) {
        if (jp == 11) {
            strncpy(b->pins[pos].name, "5V", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else if (jp == 12) {
            strncpy(b->pins[pos].name, "GND", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else {
            snprintf(b->pins[pos].name, 8, "JP7-%d", jp);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
            b->pins[pos].gpio.pinmap = os_index;
            b->pins[pos].gpio.mux_total = 0;
            os_index++;
        }
        pos++;
        jp++;
    }

    os_index = 75;
    while (pos < 112) {
        if (jp == 29) {
            strncpy(b->pins[pos].name, "3V3", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else if (jp == 30) {
            strncpy(b->pins[pos].name, "GND", 8);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
        } else {
            snprintf(b->pins[pos].name, 8, "JP7-%d", jp);
            b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
            b->pins[pos].gpio.pinmap = os_index;
            b->pins[pos].gpio.mux_total = 0;
            os_index++;
        }
        pos++;
        jp++;
    }

    // 4 Switches
    jp = 0;
    os_index = 363;
    while (pos < 116) {
        snprintf(b->pins[pos].name, 8, "SW%d", jp);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[pos].gpio.pinmap = os_index;
        b->pins[pos].gpio.mux_total = 0;
        pos++;
        jp++;
        os_index++;
    }

    // 8 LEDs
    jp = 0;
    os_index = 395;
    while (pos < 124) {
        snprintf(b->pins[pos].name, 8, "LED%d", jp);
        b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[pos].gpio.pinmap = os_index;
        b->pins[pos].gpio.mux_total = 0;
        pos++;
        jp++;
        os_index++;
    }

    // HPS_LED, HPS_KEY0, KEY0, KEY1
    strncpy(b->pins[pos].name, "HPS_LED", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 478;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "HPS_KEY", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 479;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "KEY0", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 331;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "KEY1", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 332;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    // Bus definitions
    // No function muxing needed for buses (stubbed pins if added later)
    b->no_bus_mux = 1;

    // 3 I2C buses (default 2 from Arduino header)
    b->i2c_bus_count = 3;
    b->def_i2c_bus = 2;

    b->i2c_bus[0].bus_id = 0;
    //b->i2c_bus[0].sda = 0;
    //b->i2c_bus[0].scl = 0;

    b->i2c_bus[1].bus_id = 1;
    //b->i2c_bus[1].sda = 0;
    //b->i2c_bus[1].scl = 0;

    b->i2c_bus[2].bus_id = 2;
    //b->i2c_bus[2].sda = 0;
    //b->i2c_bus[2].scl = 0;

    // 1 SPI bus
    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 32766;
    b->spi_bus[0].slave_s = 0;
    //b->spi_bus[0].cs = 0;
    //b->spi_bus[0].mosi = 0;
    //b->spi_bus[0].miso = 0;
    //b->spi_bus[0].sclk = 0;

    // Arduino header UART (default)
    b->uart_dev_count = 1;
    b->def_uart_dev = 0;
    b->uart_dev[0].device_path = "/dev/ttyS1";
    //b->uart_dev[0].rx = 0;
    //b->uart_dev[0].tx = 0;

    return b;
}
