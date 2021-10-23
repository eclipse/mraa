/*
 * Author: Srinivas Kandagatla <srinivas.kandagatla@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 * Copyright (c) 2015 Linaro Limited.
 *
 * Copied from src/arm/banana.c
 *
 * SPDX-License-Identifier: MIT
 */

#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "arm/96boards.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"

#define PLATFORM_NAME_BBGUM "BBGUM"
#define PLATFORM_NAME_DB410C "DB410C"
#define PLATFORM_NAME_DB820C "DB820C"
#define PLATFORM_NAME_HIKEY "HIKEY"
#define PLATFORM_NAME_HIKEY960 "HIKEY960"
#define PLATFORM_NAME_ROCK960 "ROCK960"
#define PLATFORM_NAME_ULTRA96 "ULTRA96"

#define MAX_SIZE 64
#define MMAP_PATH "/dev/mem"
#define DB410C_MMAP_REG 0x01000000

// Bubblegum-96
int bbgum_ls_gpio_pins[MRAA_96BOARDS_LS_GPIO_COUNT] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 155, 154 };

const char* bbgum_serialdev[MRAA_96BOARDS_LS_UART_COUNT] = { "/dev/ttyS3", "/dev/ttyS5" };

// Dragonboard410c
int db410c_ls_gpio_pins[MRAA_96BOARDS_LS_GPIO_COUNT] = {
    36, 12, 13, 69, 115, 4, 24, 25, 35, 34, 28, 33,
};

int db410c_chardev_map[MRAA_96BOARDS_LS_GPIO_COUNT][2] = {
    { 0, 36 }, { 0, 12 }, { 0, 13 }, { 0, 69 }, { 0, 115 }, { 2, 3 },
    { 0, 24 }, { 0, 25 }, { 0, 35 }, { 0, 34 }, { 0, 28 },  { 0, 33 },
};

const char* db410c_serialdev[MRAA_96BOARDS_LS_UART_COUNT] = { "/dev/ttyMSM0", "/dev/ttyMSM1" };
const char* db410c_led[MRAA_96BOARDS_LED_COUNT] = { "user1", "user2", "user3",
                                                    "user4", "bt", "wlan" };

// Dragonboard820c
int db820c_ls_gpio_pins[MRAA_96BOARDS_LS_GPIO_COUNT] = {
    80, 29, 124, 24, 62, 507, 10, 8, 25, 26, 23, 133,
};

int db820c_chardev_map[MRAA_96BOARDS_LS_GPIO_COUNT][2] = {
    { 0, 80 }, { 0, 29 }, { 0, 125 }, { 0, 24 }, { 0, 62 }, { 1, 4 },
    { 0, 10 }, { 0, 8 },  { 0, 25 },  { 0, 26 }, { 0, 23 }, { 0, 133 },
};

const char* db820c_serialdev[MRAA_96BOARDS_LS_UART_COUNT] = { "/dev/ttyMSM0", "/dev/ttyMSM1" };

// HiKey
int hikey_ls_gpio_pins[MRAA_96BOARDS_LS_GPIO_COUNT] = {
    488, 489, 490, 491, 492, 415, 463, 495, 426, 433, 427, 434,
};

int hikey_chardev_map[MRAA_96BOARDS_LS_GPIO_COUNT][2] = {
    { 2, 0 }, { 2, 1 }, { 2, 2 },  { 2, 3 }, { 2, 4 },  { 12, 7 },
    { 6, 7 }, { 2, 7 }, { 10, 2 }, { 9, 1 }, { 10, 3 }, { 9, 2 },
};

const char* hikey_serialdev[MRAA_96BOARDS_LS_UART_COUNT] = { "/dev/ttyAMA2", "/dev/ttyAMA3" };

// HiKey960
int hikey960_chardev_map[MRAA_96BOARDS_LS_GPIO_COUNT][2] = {
    { 26, 0 }, { 26, 1 }, { 26, 2 }, { 26, 3 }, { 26, 4 }, { 22, 6 },
    { 2, 7 },  { 5, 0 },  { 6, 4 },  { 2, 3 },  { 9, 3 },  { 2, 5 },
};

const char* hikey960_serialdev[MRAA_96BOARDS_LS_UART_COUNT] = { "/dev/ttyAMA3", "/dev/ttyAMA6" };

// Rock960
int rock960_ls_gpio_pins[MRAA_96BOARDS_LS_GPIO_COUNT] = { 1006, 1002, 1041, 1042, 1121, 1128,
                                                          1124, 1131, 1125, 1132, 1050, 1055 };

int rock960_chardev_map[MRAA_96BOARDS_LS_GPIO_COUNT][2] = {
    { 0, 6 },  { 0, 2 }, { 1, 9 },  { 1, 10 }, { 3, 25 }, { 4, 0 },
    { 3, 28 }, { 4, 3 }, { 3, 29 }, { 4, 4 },  { 1, 18 }, { 1, 23 },
};

const char* rock960_serialdev[MRAA_96BOARDS_LS_UART_COUNT] = { "/dev/ttyS3", "/dev/ttyS4" };

// Ultra96
int ultra96_chardev_map[MRAA_96BOARDS_LS_GPIO_COUNT][2] = {
    { 0, 36 }, { 0, 37 }, { 0, 39 }, { 0, 40 }, { 0, 44 }, { 0, 45 },
    { 0, 78 }, { 0, 79 }, { 0, 80 }, { 0, 81 }, { 0, 82 }, { 0, 83 },
};

const char* ultra96_serialdev[MRAA_96BOARDS_LS_UART_COUNT] = { "/dev/ttyPS0", "/dev/ttyS0" };

// MMAP
static uint8_t* mmap_reg = NULL;
static int mmap_fd = 0;
static int mmap_size = 0x00120004;
static unsigned int mmap_count = 0;

void
mraa_96boards_pininfo(mraa_board_t* board, int index, int sysfs_pin, int is_gpio, char* fmt, ...)
{
    va_list arg_ptr;
    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    va_start(arg_ptr, fmt);
    vsnprintf(pininfo->name, MRAA_PIN_NAME_SIZE, fmt, arg_ptr);
    if (is_gpio) {
        // skip the read argument
        va_arg(arg_ptr, int);
        pininfo->gpio.gpio_chip = va_arg(arg_ptr, int);
        pininfo->gpio.gpio_line = va_arg(arg_ptr, int);
        pininfo->capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    } else {
        pininfo->capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };
    }
    va_end(arg_ptr);
    pininfo->gpio.pinmap = sysfs_pin;
    pininfo->gpio.mux_total = 0;
}

static mraa_result_t
mraa_db410c_mmap_unsetup()
{
    if (mmap_reg == NULL) {
        syslog(LOG_WARNING, "db410c mmap: null register nothing to unsetup");
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    munmap(mmap_reg, mmap_size);
    mmap_reg = NULL;
    close(mmap_fd);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_db410c_mmap_write(mraa_gpio_context dev, int value)
{
    uint32_t offset = (0x1000 * dev->pin);

    if (value) {
        *(volatile uint32_t*) (mmap_reg + offset + 0x04) |= (uint32_t)(1 << 1);
    } else {
        *(volatile uint32_t*) (mmap_reg + offset + 0x04) &= ~(uint32_t)(1 << 1);
    }

    return MRAA_SUCCESS;
}

int
mraa_db410c_mmap_read(mraa_gpio_context dev)
{
    uint32_t value;
    uint32_t offset = (0x1000 * dev->pin);

    value = *(volatile uint32_t*) (mmap_reg + offset + 0x04);
    if (value & (uint32_t)(1 << 0)) {
        return 1;
    }

    return 0;
}

mraa_result_t
mraa_db410c_mmap_setup(mraa_gpio_context dev, mraa_boolean_t en)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "db410c mmap: context not valid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    /* disable mmap if already enabled */
    if (en == 0) {
        if (dev->mmap_write == NULL && dev->mmap_read == NULL) {
            syslog(LOG_ERR, "db410c mmap: can't disable disabled mmap gpio");
            return MRAA_ERROR_INVALID_PARAMETER;
        }
        dev->mmap_write = NULL;
        dev->mmap_read = NULL;
        mmap_count--;
        if (mmap_count == 0) {
            return mraa_db410c_mmap_unsetup();
        }
        return MRAA_SUCCESS;
    }

    if (dev->mmap_write != NULL && dev->mmap_read != NULL) {
        syslog(LOG_ERR, "db410c mmap: can't enable enabled mmap gpio");
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    if (mmap_reg == NULL) {
        if ((mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
            syslog(LOG_ERR, "db410c mmap: unable to open /dev/mem");
            return MRAA_ERROR_INVALID_HANDLE;
        }
        mmap_reg = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd, DB410C_MMAP_REG);

        if (mmap_reg == MAP_FAILED) {
            syslog(LOG_ERR, "db410c mmap: failed to mmap");
            mmap_reg = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    dev->mmap_write = &mraa_db410c_mmap_write;
    dev->mmap_read = &mraa_db410c_mmap_read;
    mmap_count++;

    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_96boards()
{
    int i;
    int* ls_gpio_pins = NULL;
    int(*chardev_map)[MRAA_96BOARDS_LS_GPIO_COUNT][2] = NULL;

    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b);
        return NULL;
    }

    // pin mux for buses are setup by default by kernel so tell mraa to ignore them
    b->no_bus_mux = 1;
    b->phy_pin_count = MRAA_96BOARDS_LS_PIN_COUNT + 1;

    if (mraa_file_exist(DT_BASE "/model")) {
        // We are on a modern kernel, great!!!!
        if (mraa_file_contains(DT_BASE "/model", "s900")) {
            b->platform_name = PLATFORM_NAME_BBGUM;
            ls_gpio_pins = bbgum_ls_gpio_pins;
            b->uart_dev[0].device_path = (char*) bbgum_serialdev[0];
            b->uart_dev[1].device_path = (char*) bbgum_serialdev[1];
        } else if (mraa_file_contains(DT_BASE "/model",
                                      "Qualcomm Technologies, Inc. APQ 8016 SBC")) {
            b->platform_name = PLATFORM_NAME_DB410C;
            ls_gpio_pins = db410c_ls_gpio_pins;
            chardev_map = &db410c_chardev_map;
            b->uart_dev[0].device_path = (char*) db410c_serialdev[0];
            b->uart_dev[1].device_path = (char*) db410c_serialdev[1];
            b->led_dev[0].name = (char*) db410c_led[0];
            b->led_dev[1].name = (char*) db410c_led[1];
            b->led_dev[2].name = (char*) db410c_led[2];
            b->led_dev[3].name = (char*) db410c_led[3];
            b->led_dev[4].name = (char*) db410c_led[4];
            b->led_dev[5].name = (char*) db410c_led[5];
            b->led_dev_count = MRAA_96BOARDS_LED_COUNT;
            b->adv_func->gpio_mmap_setup = &mraa_db410c_mmap_setup;
            b->chardev_capable = 1;
        } else if (mraa_file_contains(DT_BASE "/model", "Qualcomm Technologies, Inc. DB820c")) {
            b->platform_name = PLATFORM_NAME_DB820C;
            ls_gpio_pins = db820c_ls_gpio_pins;
            chardev_map = &db820c_chardev_map;
            b->uart_dev[0].device_path = (char*) db820c_serialdev[0];
            b->uart_dev[1].device_path = (char*) db820c_serialdev[1];
            b->chardev_capable = 1;
        } else if (mraa_file_contains(DT_BASE "/model", "HiKey Development Board")) {
            b->platform_name = PLATFORM_NAME_HIKEY;
            ls_gpio_pins = hikey_ls_gpio_pins;
            chardev_map = &hikey_chardev_map;
            b->uart_dev[0].device_path = (char*) hikey_serialdev[0];
            b->uart_dev[1].device_path = (char*) hikey_serialdev[1];
            b->chardev_capable = 1;
        } else if (mraa_file_contains(DT_BASE "/model", "HiKey960")) {
            b->platform_name = PLATFORM_NAME_HIKEY960;
            chardev_map = &hikey960_chardev_map;
            b->uart_dev[0].device_path = (char*) hikey960_serialdev[0];
            b->uart_dev[1].device_path = (char*) hikey960_serialdev[1];
            b->chardev_capable = 1;
        } else if (mraa_file_contains(DT_BASE "/model", "ROCK960")) {
            b->platform_name = PLATFORM_NAME_ROCK960;
            ls_gpio_pins = rock960_ls_gpio_pins;
            chardev_map = &rock960_chardev_map;
            b->uart_dev[0].device_path = (char*) rock960_serialdev[0];
            b->uart_dev[1].device_path = (char*) rock960_serialdev[1];
            b->chardev_capable = 1;
        } else if ((mraa_file_contains(DT_BASE "/model", "ZynqMP ZCU100 RevC")) ||
                   (mraa_file_contains(DT_BASE "/model", "Avnet Ultra96 Rev1"))) {
            b->platform_name = PLATFORM_NAME_ULTRA96;
            chardev_map = &ultra96_chardev_map;
            b->uart_dev[0].device_path = (char*) ultra96_serialdev[0];
            b->uart_dev[1].device_path = (char*) ultra96_serialdev[1];
            b->chardev_capable = 1;
        }
    }

    // UART
    b->uart_dev_count = MRAA_96BOARDS_LS_UART_COUNT;
    b->def_uart_dev = 0;

    // I2C
    if (strncmp(b->platform_name, PLATFORM_NAME_BBGUM, MAX_SIZE) == 0) {
        b->i2c_bus_count = MRAA_96BOARDS_LS_I2C_COUNT;
        b->def_i2c_bus = 0;
        b->i2c_bus[0].bus_id = 1;
        b->i2c_bus[1].bus_id = 2;
    } else if (strncmp(b->platform_name, PLATFORM_NAME_ROCK960, MAX_SIZE) == 0) {
        b->i2c_bus_count = MRAA_96BOARDS_LS_I2C_COUNT;
        b->def_i2c_bus = 0;
        b->i2c_bus[0].bus_id = 6;
        b->i2c_bus[1].bus_id = 1;
    } else if (strncmp(b->platform_name, PLATFORM_NAME_ULTRA96, MAX_SIZE) == 0) {
        b->i2c_bus_count = MRAA_96BOARDS_LS_I2C_COUNT;
        b->def_i2c_bus = 0;
        b->i2c_bus[0].bus_id = 2;
        b->i2c_bus[1].bus_id = 3;
    } else {
        b->i2c_bus_count = MRAA_96BOARDS_LS_I2C_COUNT;
        b->def_i2c_bus = 0;
        b->i2c_bus[0].bus_id = 0;
        b->i2c_bus[1].bus_id = 1;
    }

    // SPI
    b->spi_bus_count = MRAA_96BOARDS_LS_SPI_COUNT;
    b->spi_bus[0].bus_id = 0;
    b->def_spi_bus = 0;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    mraa_96boards_pininfo(b, 0, -1, 0, "INVALID");
    mraa_96boards_pininfo(b, 1, -1, 0, "GND");
    mraa_96boards_pininfo(b, 2, -1, 0, "GND");
    mraa_96boards_pininfo(b, 3, -1, 0, "UART0_CTS");
    mraa_96boards_pininfo(b, 4, -1, 0, "PWR_BTN_N");
    mraa_96boards_pininfo(b, 5, -1, 0, "UART0_TXD");
    mraa_96boards_pininfo(b, 6, -1, 0, "RST_BTN_N");
    mraa_96boards_pininfo(b, 7, -1, 0, "UART0_RXD");
    mraa_96boards_pininfo(b, 8, -1, 0, "SPI0_SCLK");
    mraa_96boards_pininfo(b, 9, -1, 0, "UART0_RTS");
    mraa_96boards_pininfo(b, 10, -1, 0, "SPI0_DIN");
    mraa_96boards_pininfo(b, 11, -1, 0, "UART1_TXD");
    // On DB410c, configure the SPI0_CS pin as GPIO for enabling the
    // user to control it manually without adding chip select property
    // in Devicetree.
    if (strncmp(b->platform_name, PLATFORM_NAME_DB410C, MAX_SIZE) == 0) {
        mraa_96boards_pininfo(b, 12, 18, 1, "SPI0_CS", -1, 0, 18);
    } else {
        mraa_96boards_pininfo(b, 12, -1, 0, "SPI0_CS");
    }
    mraa_96boards_pininfo(b, 13, -1, 0, "UART1_RXD");
    mraa_96boards_pininfo(b, 14, -1, 0, "SPI0_DOUT");
    mraa_96boards_pininfo(b, 15, -1, 0, "I2C0_SCL");
    mraa_96boards_pininfo(b, 16, -1, 0, "PCM_FS");
    mraa_96boards_pininfo(b, 17, -1, 0, "I2C0_SDA");
    mraa_96boards_pininfo(b, 18, -1, 0, "PCM_CLK");
    mraa_96boards_pininfo(b, 19, -1, 0, "I2C1_SCL");
    mraa_96boards_pininfo(b, 20, -1, 0, "PCM_DO");
    mraa_96boards_pininfo(b, 21, -1, 0, "I2C1_SDA");
    mraa_96boards_pininfo(b, 22, -1, 0, "PCM_DI");
    // GPIOs are labelled "GPIO-A" through "GPIO-L"
    for (i = 0; i < MRAA_96BOARDS_LS_GPIO_COUNT; i++) {
        mraa_96boards_pininfo(b, 23 + i, ls_gpio_pins ? ls_gpio_pins[i] : -1, 1, "GPIO-%c", 'A' + i,
                              chardev_map ? (*chardev_map)[i][0] : -1,
                              chardev_map ? (*chardev_map)[i][1] : -1);
    }
    mraa_96boards_pininfo(b, 35, -1, 0, "1.8v");
    mraa_96boards_pininfo(b, 36, -1, 0, "SYS_DCIN");
    mraa_96boards_pininfo(b, 37, -1, 0, "5v");
    mraa_96boards_pininfo(b, 38, -1, 0, "SYS_DCIN");
    mraa_96boards_pininfo(b, 39, -1, 0, "GND");
    mraa_96boards_pininfo(b, 40, -1, 0, "GND");

    // On DB410c, SPI0_CS pin is used as GPIO
    if (strncmp(b->platform_name, PLATFORM_NAME_DB410C, MAX_SIZE) == 0) {
        b->gpio_count = MRAA_96BOARDS_LS_GPIO_COUNT + 1;
    } else {
        b->gpio_count = MRAA_96BOARDS_LS_GPIO_COUNT;
    }

    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;

    return b;
}
