/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "types.h"

/** @file
 *
 * This file defines the basic shared values for libmraa
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MRAA boolean type
 * 1 For TRUE
 */
typedef unsigned int mraa_boolean_t;

/**
 * Enum representing different possible modes for a pin.
 */
typedef enum {
    MRAA_PIN_VALID       = 0, /**< Pin Valid */
    MRAA_PIN_GPIO        = 1, /**< General Purpose IO */
    MRAA_PIN_PWM         = 2, /**< Pulse Width Modulation */
    MRAA_PIN_FAST_GPIO   = 3, /**< Faster GPIO */
    MRAA_PIN_SPI         = 4, /**< SPI */
    MRAA_PIN_I2C         = 5, /**< I2C */
    MRAA_PIN_AIO         = 6, /**< Analog in */
    MRAA_PIN_UART        = 7  /**< UART */
} mraa_pinmodes_t;

/**
 * A bitfield representing the capabilities of a pin.
 */
typedef struct {
    /*@{*/
    mraa_boolean_t valid:1;     /**< Is the pin valid at all */
    mraa_boolean_t gpio:1;      /**< Is the pin gpio capable */
    mraa_boolean_t pwm:1;       /**< Is the pin pwm capable */
    mraa_boolean_t fast_gpio:1; /**< Is the pin fast gpio capable */
    mraa_boolean_t spi:1;       /**< Is the pin spi capable */
    mraa_boolean_t i2c:1;       /**< Is the pin i2c capable */
    mraa_boolean_t aio:1;       /**< Is the pin analog input capable */
    mraa_boolean_t uart:1;       /**< Is the pin uart capable */
    /*@}*/
} mraa_pincapabilities_t;

/**
 * A Structure representing a multiplexer and the required value
 */
typedef struct {
    /*@{*/
    unsigned int pin;   /**< Raw GPIO pin id */
    unsigned int value; /**< Raw GPIO value */
    /*@}*/
} mraa_mux_t;

typedef struct {
    mraa_boolean_t complex_pin:1;
    mraa_boolean_t output_en:1;
    mraa_boolean_t output_en_high:1;
    mraa_boolean_t pullup_en:1;
    mraa_boolean_t pullup_en_hiz:1;
} mraa_pin_cap_complex_t;

typedef struct {
    /*@{*/
    unsigned int pinmap; /**< sysfs pin */
    unsigned int parent_id; /** parent chip id */
    unsigned int mux_total; /** Numfer of muxes needed for operation of pin */
    mraa_mux_t mux[6]; /** Array holding information about mux */
    unsigned int output_enable; /** Output Enable GPIO, for level shifting */
    unsigned int pullup_enable; /** Pull-Up enable GPIO, inputs */
    mraa_pin_cap_complex_t complex_cap;
    /*@}*/
} mraa_pin_t;

typedef struct {
    /*@{*/
    char mem_dev[32]; /**< Memory device to use /dev/uio0 etc */
    unsigned int mem_sz; /** Size of memory to map */
    unsigned int bit_pos; /** Position of value bit */
    mraa_pin_t gpio; /** GPio context containing none mmap info */
    /*@}*/
} mraa_mmap_pin_t;

/**
 * A Structure representing a physical Pin.
 */
typedef struct {
    /*@{*/
    char name[8];                      /**< Pin's real world name */
    mraa_pincapabilities_t capabilites; /**< Pin Capabiliites */
    mraa_pin_t gpio; /**< GPIO structure */
    mraa_pin_t pwm;  /**< PWM structure */
    mraa_pin_t aio;  /**< Anaglog Pin */
    mraa_mmap_pin_t mmap; /**< GPIO through memory */
    mraa_pin_t i2c;  /**< i2c bus/pin */
    mraa_pin_t spi;  /**< spi bus/pin */
    mraa_pin_t uart;  /**< uart module/pin */
    /*@}*/
} mraa_pininfo_t;

/**
 * A Structure representing the physical properties of a i2c bus.
 */
typedef struct {
    /*@{*/
    unsigned int bus_id; /**< ID as exposed in the system */
    unsigned int scl; /**< i2c SCL */
    unsigned int sda; /**< i2c SDA */
    /*@}*/
} mraa_i2c_bus_t;

/**
 * A Structure representing the physical properties of a spi bus.
 */
typedef struct {
    /*@{*/
    unsigned int bus_id; /**< The Bus ID as exposed to the system. */
    unsigned int slave_s; /**< Slave select */
    mraa_boolean_t three_wire; /**< Is the bus only a three wire system */
    unsigned int sclk; /**< Serial Clock */
    unsigned int mosi; /**< Master Out, Slave In. */
    unsigned int miso; /**< Master In, Slave Out. */
    unsigned int cs; /**< Chip Select, used when the board is a spi slave */
    /*@}*/
} mraa_spi_bus_t;

/**
 * A Structure representing a uart device.
 */
typedef struct {
    /*@{*/
    unsigned int index; /**< ID as exposed in the system */
    int rx; /**< uart rx */
    int tx; /**< uart tx */
    /*@}*/
} mraa_uart_dev_t;

/**
 * A Structure representing a platform/board.
 */
typedef struct {
    /*@{*/
    unsigned int phy_pin_count; /**< The Total IO pins on board */
    unsigned int gpio_count; /**< GPIO Count */
    unsigned int aio_count;  /**< Analog side Count */
    unsigned int i2c_bus_count; /**< Usable i2c Count */
    mraa_i2c_bus_t  i2c_bus[12]; /**< Array of i2c */
    unsigned int def_i2c_bus; /**< Position in array of default i2c bus */
    unsigned int spi_bus_count; /**< Usable spi Count */
    mraa_spi_bus_t spi_bus[12];       /**< Array of spi */
    unsigned int def_spi_bus; /**< Position in array of defult spi bus */
    unsigned int adc_raw; /**< ADC raw bit value */
    unsigned int adc_supported; /**< ADC supported bit value */
    unsigned int def_uart_dev; /**< Position in array of defult uart */
    unsigned int uart_dev_count; /**< Usable spi Count */
    mraa_uart_dev_t uart_dev[6]; /**< Array of UARTs */
    mraa_pininfo_t* pins;     /**< Pointer to pin array */
    /*@}*/
} mraa_board_t;

/**
 * Initialise MRAA
 *
 * Detects running platform and attempts to use included pinmap
 *
 * @return Result of operation
 */
#if (defined SWIGPYTHON) || (defined SWIG)
mraa_result_t mraa_init();
#else
// this sets a compiler attribute (supported by GCC & clang) to have mraa_init()
// be called as a constructor make sure your libc supports this!  uclibc needs
// to be compiled with UCLIBC_CTOR_DTOR
mraa_result_t mraa_init() __attribute__((constructor));
#endif

/**
 * De-Initilise MRAA
 *
 * This is not a strict requirement but useful to test memory leaks and for
 * people who like super clean code.
 */
void mraa_deinit();

/**
 * Checks if a pin is able to use the passed in mode.
 *
 * @param pin Physical Pin to be checked.
 * @param mode the mode to be tested.
 * @return boolean if the mode is supported, 0=false.
 */
mraa_boolean_t mraa_pin_mode_test(int pin, mraa_pinmodes_t mode);

/**
 * Check the board's  bit size when reading the value
 *
 * @return raw bits being read from kernel module. zero if no ADC
 */
unsigned int mraa_adc_raw_bits();

/**
 * Return value that the raw value should be shifted to. Zero if no ADC
 *
 * @return return actual bit size the adc value should be understood as.
 */
unsigned int mraa_adc_supported_bits();

#ifdef __cplusplus
}
#endif
