/*
 * Author: Brian <brian@vamrs.com>
 * Author: Paul <paul.chang@mrshim.de>
 * Copyright (c) 2019 Vamrs Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "arm/rockpi.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"

/*
* "Radxa ROCK Pi 4" is the model name on stock 5.x kernels
* "ROCK PI 4A", "ROCK PI 4B" and "ROCK PI 4C" is used on Radxa 4.4 kernel
* so we search for the string below by ignoring case
*/
#define PLATFORM_NAME_ROCK_PI4 "ROCK Pi 4"
#define PLATFORM_NAME_ROCK_PI4_2 "ROCK PI 4"
#define PLATFORM_NAME_ROCK_PI4_3 "ROCK 4"

#define PLATFORM_NAME_ROCK_PIS "ROCK Pi S"

#define MAX_SIZE 64
#define MAX_PERIPH 4

typedef struct
{
    int serialdev_index[MAX_PERIPH];
    int i2cbus_index[MAX_PERIPH];
    int spibus_index[MAX_PERIPH];
    int pwmdev_index[MAX_PERIPH];
} PeripheralIndex;

PeripheralIndex rockpi_index[ROCKPI_NUM_SUPPORTED_HW] = {
    // Peripheral indices for Rock Pi 4
    {
        .serialdev_index = { 2, 4 },
        .i2cbus_index = { 7, 2, 6 },
        .spibus_index = { 1, 2 },
        .pwmdev_index = { 0, 1 }
    },

    // Peripheral indices for Rock Pi S
    {
        .serialdev_index = { 0, 1, 2 },
        .i2cbus_index = { 1, 0, 3 },
        .spibus_index = { 2 },
        .pwmdev_index = { 2, 3 }
    }
};

void
mraa_rockpi_pininfo(mraa_board_t* board, int index, int sysfs_pin, mraa_pincapabilities_t pincapabilities_t, char* fmt, ...)
{
    va_list arg_ptr;
    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    va_start(arg_ptr, fmt);
    vsnprintf(pininfo->name, MRAA_PIN_NAME_SIZE, fmt, arg_ptr);

    if( pincapabilities_t.gpio == 1 ) {
        pininfo->gpio.gpio_chip = sysfs_pin / 32;
        pininfo->gpio.gpio_line = sysfs_pin % 32;
    }

    pininfo->capabilities = pincapabilities_t;

    va_end(arg_ptr);
    pininfo->gpio.pinmap = sysfs_pin;
    pininfo->gpio.mux_total = 0;
}

void
rockpi4_pins(mraa_board_t* b)
{
    mraa_rockpi_pininfo(b, 0,   -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_rockpi_pininfo(b, 1,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpi_pininfo(b, 2,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpi_pininfo(b, 3,   71, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "SDA7");
    mraa_rockpi_pininfo(b, 4,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpi_pininfo(b, 5,   72, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "SCL7");
    mraa_rockpi_pininfo(b, 6,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 7,   75, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI2_CLK");
    mraa_rockpi_pininfo(b, 8,  148, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "TXD2");
    mraa_rockpi_pininfo(b, 9,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 10, 147, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "RXD2");
    mraa_rockpi_pininfo(b, 11, 146, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "PWM0");
    mraa_rockpi_pininfo(b, 12, 131, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_A3");
    mraa_rockpi_pininfo(b, 13, 150, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "PWM1");
    mraa_rockpi_pininfo(b, 14,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 15, 149, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_C5");
    mraa_rockpi_pininfo(b, 16, 154, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_D2");
    mraa_rockpi_pininfo(b, 17,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpi_pininfo(b, 18, 156, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_D4");
    mraa_rockpi_pininfo(b, 19,  40, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "SPI1TX,TXD4");
    mraa_rockpi_pininfo(b, 20,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 21,  39, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "SPI1RX,RXD4");
    mraa_rockpi_pininfo(b, 22, 157, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_D5");
    mraa_rockpi_pininfo(b, 23,  41, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI1CLK");
    mraa_rockpi_pininfo(b, 24,  42, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI1CS");
    mraa_rockpi_pininfo(b, 25,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 26,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "ADC_IN0");
    mraa_rockpi_pininfo(b, 27,  64, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "SDA2");
    mraa_rockpi_pininfo(b, 28,  65, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "SCL2");
    mraa_rockpi_pininfo(b, 29,  74, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "SCL6,SPI2RX");
    mraa_rockpi_pininfo(b, 30,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 31,  73, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "SDA6,SPI2TX");
    mraa_rockpi_pininfo(b, 32, 112, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_C0");
    mraa_rockpi_pininfo(b, 33,  76, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI2CS");
    mraa_rockpi_pininfo(b, 34,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 35, 133, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_A5");
    mraa_rockpi_pininfo(b, 36, 132, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_A4");
    mraa_rockpi_pininfo(b, 37, 158, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_D6");
    mraa_rockpi_pininfo(b, 38, 134, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_A6");
    mraa_rockpi_pininfo(b, 39,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 40, 135, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO4_A7");
}

void
rockpiS_pins(mraa_board_t* b)
{
    // GPIO pin names correspond to boards V12 and V13
    mraa_rockpi_pininfo(b, 0,   -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_rockpi_pininfo(b, 1,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpi_pininfo(b, 2,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpi_pininfo(b, 3,   11, (mraa_pincapabilities_t){1,0,0,0,0,1,0,0}, "SDA1");
    mraa_rockpi_pininfo(b, 4,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpi_pininfo(b, 5,   12, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "SCL1");
    mraa_rockpi_pininfo(b, 6,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 7,   68, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_A4");
    mraa_rockpi_pininfo(b, 8,   65, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "TXD0");
    mraa_rockpi_pininfo(b, 9,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 10,  64, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "RXD0");
    mraa_rockpi_pininfo(b, 11,  15, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "PWM2");
    mraa_rockpi_pininfo(b, 12,  69, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_A5");
    mraa_rockpi_pininfo(b, 13,  16, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "PWM3");
    mraa_rockpi_pininfo(b, 14,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 15,  17, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO0_C1");
    mraa_rockpi_pininfo(b, 16,  74, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B2");
    mraa_rockpi_pininfo(b, 17,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpi_pininfo(b, 18,  73, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B1");
    mraa_rockpi_pininfo(b, 19,  55, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "SPI2TX,TXD2");
    mraa_rockpi_pininfo(b, 20,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 21,  54, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "SPI2RX,RXD2");
    mraa_rockpi_pininfo(b, 22,  71, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_A7");
    mraa_rockpi_pininfo(b, 23,  56, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "CK2,SDA0,RX1");
    mraa_rockpi_pininfo(b, 24,  57, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "CS2,SCL0,TX1");
    mraa_rockpi_pininfo(b, 25,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 26,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "ADC_IN0");

    // Only use GPIO pins that are valid for all board versions: V11 to V13
    mraa_rockpi_pininfo(b, 27,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpi_pininfo(b, 28,  77, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B5");
    mraa_rockpi_pininfo(b, 29,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "ADC_KEY_IN1");
    mraa_rockpi_pininfo(b, 30,  78, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B6");
    mraa_rockpi_pininfo(b, 31,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICBIAS2");
    mraa_rockpi_pininfo(b, 32,  79, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_B7");
    mraa_rockpi_pininfo(b, 33,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICBIAS1");
    mraa_rockpi_pininfo(b, 34,  80, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO2_C0");
    mraa_rockpi_pininfo(b, 35,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN8");
    mraa_rockpi_pininfo(b, 36,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP8");
    mraa_rockpi_pininfo(b, 37,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN7");
    mraa_rockpi_pininfo(b, 38,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP7");
    mraa_rockpi_pininfo(b, 39,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN6");
    mraa_rockpi_pininfo(b, 40,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP6");
    mraa_rockpi_pininfo(b, 41,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN5");
    mraa_rockpi_pininfo(b, 42,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP5");
    mraa_rockpi_pininfo(b, 43,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN4");
    mraa_rockpi_pininfo(b, 44,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP4");
    mraa_rockpi_pininfo(b, 45,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN3");
    mraa_rockpi_pininfo(b, 46,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP3");
    mraa_rockpi_pininfo(b, 47,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN2");
    mraa_rockpi_pininfo(b, 48,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP2");
    mraa_rockpi_pininfo(b, 49,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MICN1");
    mraa_rockpi_pininfo(b, 50,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "MCIP1");
    mraa_rockpi_pininfo(b, 51,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "LINEOUT_R");
    mraa_rockpi_pininfo(b, 52,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "LINEOUT_L");
}

mraa_board_t*
mraa_rockpi()
{
    int i, model_index;
    char *serialdev_path;
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

    if (mraa_file_exist(DT_BASE "/model")) {
        // We are on a modern kernel, great!!!!
        if (mraa_file_contains(DT_BASE "/model", PLATFORM_NAME_ROCK_PI4)  ||
            mraa_file_contains(DT_BASE "/model", PLATFORM_NAME_ROCK_PI4_2) ||
            mraa_file_contains(DT_BASE "/model", PLATFORM_NAME_ROCK_PI4_3)
            ) {
            model_index = MRAA_ROCKPI4_INDEX;
            b->phy_pin_count = MRAA_ROCKPI4_PIN_COUNT + 1;
            b->platform_name = PLATFORM_NAME_ROCK_PI4;
            b->uart_dev_count = MRAA_ROCKPI4_UART_COUNT;
            b->spi_bus_count = MRAA_ROCKPI4_SPI_COUNT;
            b->pwm_dev_count = MRAA_ROCKPI4_PWM_COUNT;
            b->aio_count = MRAA_ROCKPI4_AIO_COUNT;

            if (strncmp(b->platform_name, PLATFORM_NAME_ROCK_PI4, MAX_SIZE) == 0) {
                b->i2c_bus_count = MRAA_ROCKPI4_I2C_COUNT;
            }

        // Rock Pi S
        } else if (mraa_file_contains(DT_BASE "/model", PLATFORM_NAME_ROCK_PIS)) {
            model_index = MRAA_ROCKPIS_INDEX;
            b->phy_pin_count = MRAA_ROCKPIS_PIN_COUNT + 1;
            b->platform_name = PLATFORM_NAME_ROCK_PIS;
            b->uart_dev_count = MRAA_ROCKPIS_UART_COUNT;
            b->spi_bus_count = MRAA_ROCKPIS_SPI_COUNT;
            b->pwm_dev_count = MRAA_ROCKPIS_PWM_COUNT;
            b->aio_count = MRAA_ROCKPIS_AIO_COUNT;
            b->i2c_bus_count = MRAA_ROCKPIS_I2C_COUNT;
        }
    }

    // UART
    b->def_uart_dev = 0;
    for (i = 0; i < b->uart_dev_count; i++) {
        b->uart_dev[i].index = rockpi_index[model_index].serialdev_index[i];
        sprintf(serialdev_path, "/dev/ttyS%i", b->uart_dev[i].index);
        b->uart_dev[i].device_path = serialdev_path;
    }

    // I2C
    b->def_i2c_bus = 0;
    for (i = 0; i < b->spi_bus_count; i++) {
        b->i2c_bus[i].bus_id = rockpi_index[model_index].i2cbus_index[i];
    }

    // SPI
    b->def_spi_bus = 0;
    for (i = 0; i < b->spi_bus_count; i++) {
        b->spi_bus[i].bus_id = rockpi_index[model_index].spibus_index[i];
    }

    // PWM
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[11].pwm.parent_id = rockpi_index[model_index].pwmdev_index[0];
    b->pins[11].pwm.mux_total = 0;
    b->pins[11].pwm.pinmap = 0;
    b->pins[13].pwm.parent_id = rockpi_index[model_index].pwmdev_index[1];
    b->pins[13].pwm.mux_total = 0;
    b->pins[13].pwm.pinmap = 0;

    // ADC
    b->adc_raw = 10;
    b->adc_supported = 10;
    b->aio_dev[0].pin = 26;
    b->aio_non_seq = 1;
    b->chardev_capable = 1;

    // Define pin mappings
    switch (model_index) {
        case MRAA_ROCKPI4_INDEX:
            rockpi4_pins(b);
            break;

        case MRAA_ROCKPIS_INDEX:
            rockpiS_pins(b);
            break;
    }

    return b;
}
