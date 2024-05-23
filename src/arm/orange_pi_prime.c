/*
 * Author: Gunjan <viraniac@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "arm/orange_pi_prime.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"

#define PLATFORM_NAME_ORANGE_PI_PRIME "Xunlong Orange Pi Prime"
#define MAX_SIZE 64

#define ORANGE_PI_PRIME_SPI_CS_DEV "/dev/spidev1.1"

#define ORANGE_PI_PRIME_SERIALDEV "/dev/ttyS2"

void
mraa_orange_pi_prime_pininfo(mraa_board_t* board, int index, int sysfs_pin, mraa_pincapabilities_t pincapabilities_t, char* fmt, ...)
{
    va_list arg_ptr;
    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    va_start(arg_ptr, fmt);
    vsnprintf(pininfo->name, MRAA_PIN_NAME_SIZE, fmt, arg_ptr);

    if( pincapabilities_t.gpio == 1 ) {
        pininfo->gpio.gpio_chip = 1;
        pininfo->gpio.gpio_line = sysfs_pin;
    }

    pininfo->capabilities = pincapabilities_t;

    va_end(arg_ptr);
    pininfo->gpio.pinmap = sysfs_pin;
    pininfo->gpio.mux_total = 0;
}

mraa_board_t*
mraa_orange_pi_prime()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b);
        return NULL;
    }

    b->chardev_capable = 1;

    // pin mux for buses are setup using device tree overlays
    // so tell mraa to ignore them
    b->no_bus_mux = 1;
    b->phy_pin_count = MRAA_ORANGE_PI_PRIME_PIN_COUNT + 1;
    b->platform_name = PLATFORM_NAME_ORANGE_PI_PRIME;

    // uart2
    b->uart_dev_count = MRAA_ORANGE_PI_PRIME_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].device_path = ORANGE_PI_PRIME_SERIALDEV;

    // I2C
    b->i2c_bus_count = MRAA_ORANGE_PI_PRIME_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[1].bus_id = 1;

    // SPI
    b->spi_bus_count = MRAA_ORANGE_PI_PRIME_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 1;

    // Depending on the overlay parameter, we might have slave select
    if (mraa_file_exist(ORANGE_PI_PRIME_SPI_CS_DEV)) {
        b->spi_bus[0].slave_s = 1;
    }

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    // no analog I/O
    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;

    // Orange Pi Prime documentation shows pin 7 (PA6) as pwm1 pin
    // but Allwinner H5 doesn't list pwm as a function for PA6.
    // Hence no PWM for this board
    b->pwm_dev_count = 0;

    mraa_orange_pi_prime_pininfo(b,  0,  -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_orange_pi_prime_pininfo(b,  1,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_orange_pi_prime_pininfo(b,  2,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_orange_pi_prime_pininfo(b,  3,  12, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C0_SDA");
    mraa_orange_pi_prime_pininfo(b,  4,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_orange_pi_prime_pininfo(b,  5,  11, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C0_SCK");
    mraa_orange_pi_prime_pininfo(b,  6,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b,  7,   6, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PA6");
    mraa_orange_pi_prime_pininfo(b,  8,  69, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC5");
    mraa_orange_pi_prime_pininfo(b,  9,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b, 10,  70, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC6");
    mraa_orange_pi_prime_pininfo(b, 11,   1, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART2_RX");
    mraa_orange_pi_prime_pininfo(b, 12, 110, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PD14");
    mraa_orange_pi_prime_pininfo(b, 13,   0, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART2_TX");
    mraa_orange_pi_prime_pininfo(b, 14,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b, 15,   3, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART2_CTS");
    mraa_orange_pi_prime_pininfo(b, 16,  68, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC4");
    mraa_orange_pi_prime_pininfo(b, 17,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_orange_pi_prime_pininfo(b, 18,  71, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC7");
    mraa_orange_pi_prime_pininfo(b, 19,  15, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI1_MOSI");
    mraa_orange_pi_prime_pininfo(b, 20,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b, 21,  16, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI1_MISO");
    mraa_orange_pi_prime_pininfo(b, 22,   2, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART2_RTS");
    mraa_orange_pi_prime_pininfo(b, 23,  14, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI1_CLK");
    mraa_orange_pi_prime_pininfo(b, 24,  13, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "SPI1_CS");
    mraa_orange_pi_prime_pininfo(b, 25,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b, 26,  72, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC8");
    mraa_orange_pi_prime_pininfo(b, 27,  19, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C1_SDA");
    mraa_orange_pi_prime_pininfo(b, 28,  18, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C1_SCK");
    mraa_orange_pi_prime_pininfo(b, 29,   7, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PA7");
    mraa_orange_pi_prime_pininfo(b, 30,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b, 31,   8, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PA8");
    mraa_orange_pi_prime_pininfo(b, 32,  73, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC9");
    mraa_orange_pi_prime_pininfo(b, 33,   9, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PA9");
    mraa_orange_pi_prime_pininfo(b, 34,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b, 35,  10, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PA10");
    mraa_orange_pi_prime_pininfo(b, 36,  74, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC10");
    mraa_orange_pi_prime_pininfo(b, 37, 107, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PD11");
    mraa_orange_pi_prime_pininfo(b, 38,  75, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC11");
    mraa_orange_pi_prime_pininfo(b, 39,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_orange_pi_prime_pininfo(b, 40,  76, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "PC12");

    return b;
}
