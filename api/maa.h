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

/** @file
 *
 * This file defines the basic shared values for libmaa
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MAA return codes
 */
typedef enum {
    MAA_SUCCESS                              =  0, /**< Expected response */
    MAA_ERROR_FEATURE_NOT_IMPLEMENTED        =  1, /**< Feature TODO */
    MAA_ERROR_FEATURE_NOT_SUPPORTED          =  2, /**< Feature not supported by HW */
    MAA_ERROR_INVALID_VERBOSITY_LEVEL        =  3, /**< Verbosity level wrong */
    MAA_ERROR_INVALID_PARAMETER              =  4, /**< Parameter invalid */
    MAA_ERROR_INVALID_HANDLE                 =  5, /**< Handle invalid */
    MAA_ERROR_NO_RESOURCES                   =  6, /**< No resource of that type avail */
    MAA_ERROR_INVALID_RESOURCE               =  7, /**< Resource invalid */
    MAA_ERROR_INVALID_QUEUE_TYPE             =  8, /**< Queue type incorrect */
    MAA_ERROR_NO_DATA_AVAILABLE              =  9, /**< No data available */
    MAA_ERROR_INVALID_PLATFORM               = 10, /**< Platform not recognised */
    MAA_ERROR_PLATFORM_NOT_INITIALISED       = 11, /**< Board information not initialised */
    MAA_ERROR_PLATFORM_ALREADY_INITIALISED   = 12, /**< Board is already initialised */

    MAA_ERROR_UNSPECIFIED                    = 99 /**< Unknown Error */
} maa_result_t;

/**
 * MAA boolean type
 * 1 For TRUE
 */
typedef unsigned int maa_boolean_t;

/**
 * Enum representing different possible modes for a pin.
 */
typedef enum {
    MAA_PIN_VALID       = 0, /**< Pin Valid */
    MAA_PIN_GPIO        = 1, /**< General Purpose IO */
    MAA_PIN_PWM         = 2, /**< Pulse Width Modulation */
    MAA_PIN_FAST_GPIO   = 3, /**< Faster GPIO */
    MAA_PIN_SPI         = 4, /**< SPI */
    MAA_PIN_I2C         = 5, /**< I2C */
    MAA_PIN_AIO         = 6  /**< Analog in */
} maa_pinmodes_t;

/**
 * A bitfield representing the capabilities of a pin.
 */
typedef struct {
    /*@{*/
    maa_boolean_t valid:1;     /**< Is the pin valid at all */
    maa_boolean_t gpio:1;      /**< Is the pin gpio capable */
    maa_boolean_t pwm:1;       /**< Is the pin pwm capable */
    maa_boolean_t fast_gpio:1; /**< Is the pin fast gpio capable */
    maa_boolean_t spi:1;       /**< Is the pin spi capable */
    maa_boolean_t i2c:1;       /**< Is the pin i2c capable */
    maa_boolean_t aio:1;       /**< Is the pin analog input capable */
    /*@}*/
} maa_pincapabilities_t;

/**
 * A Structure representing a multiplexer and the required value
 */
typedef struct {
    /*@{*/
    unsigned int pin;   /**< Raw GPIO pin id */
    unsigned int value; /**< Raw GPIO value */
} maa_mux_t;

/**
 * A Strucutre representing a singular I/O pin. i.e GPIO/PWM
 */
typedef struct {
    /*@{*/
    unsigned int pinmap;
    unsigned int parent_id;
    unsigned int mux_total;
    maa_mux_t mux[6];
    /*@}*/
} maa_pin_t;

/**
 * A Structure representing a physical Pin.
 */
typedef struct {
    /*@{*/
    char name[8];                      /**< Pin's real world name */
    maa_pincapabilities_t capabilites; /**< Pin Capabiliites */
    maa_pin_t gpio; /**< GPIO structure */
    maa_pin_t pwm;  /**< PWM structure */
    maa_pin_t aio;  /**< Anaglog Pin */
    maa_pin_t fast_gpio; /**< Fast GPIO */
    maa_pin_t i2c;  /**< i2c bus/pin */
    maa_pin_t spi;  /**< spi bus/pin */
    /*@}*/
} maa_pininfo_t;

/**
 * A Structure representing the physical properties of a i2c bus.
 */
typedef struct {
    /*@{*/
    unsigned int bus_id; /**< ID as exposed in the system */
    unsigned int scl; /**< i2c SCL */
    unsigned int sda; /**< i2c SDA */
    /*@}*/
} maa_i2c_bus_t;

/**
 * A Structure representing the physical properties of a spi bus.
 */
typedef struct {
    /*@{*/
    double bus_id; /**< The Bus ID as exposed to the system. */
    maa_boolean_t three_wire; /**< Is the bus only a three wire system */
    unsigned int sclk; /**< Serial Clock */
    unsigned int mosi; /**< Master Out, Slave In. */
    unsigned int miso; /**< Master In, Slave Out. */
    unsigned int cs; /**< Chip Select, used when the board is a spi slave */
    /*@}*/
} maa_spi_bus_t;

/**
 * A Structure representing a platform/board.
 */
typedef struct {
    /*@{*/
    unsigned int phy_pin_count; /**< The Total IO pins on board */
    unsigned int gpio_count; /**< GPIO Count */
    unsigned int aio_count;  /**< Analog side Count */
    unsigned int i2c_bus_count; /**< Usable i2c Count */
    maa_i2c_bus_t  i2c_bus[6]; /**< Array of i2c */
    unsigned int def_i2c_bus; /**< Position in array of default i2c bus */
    unsigned int spi_bus_count; /**< Usable spi Count */
    maa_spi_bus_t spi_bus[6];       /**< Array of spi */
    unsigned int def_spi_bus; /**< Position in array of defult spi bus */
    maa_pininfo_t* pins;     /**< Pointer to pin array */
    /*@}*/
} maa_board_t;

/** Initialise MAA
 *
 * Detects running platform and attempts to use included pinmap
 * @return maa_result_t maa result
 */
#ifndef SWIG
// this sets a compiler attribute (supported by GCC & clang) to have maa_init()
// be called as a constructor make sure your libc supports this!  uclibc needs
// to be compiled with UCLIBC_CTOR_DTOR
maa_result_t maa_init() __attribute__((constructor));
#else
maa_result_t maa_init();
#endif

/** Check GPIO
 *
 * Will check input is valid for gpio and will also setup required multiplexers.
 * @param pin the pin as read from the board surface. i.e IO3 would be 3/
 * @return the pin as found in the pinmap
 */
unsigned int maa_check_gpio(int pin);

/** Check AIO
 *
 * Will check input is valid for aio and will also setup required multiplexers.
 * @param pin the pin as read from the board surface. i.e A3 would be 3/
 * @return the pin as found in the pinmap
 */
unsigned int maa_check_aio(int pin);

/** Check i2c interface, sets up multiplexer on device.
 *
 * @return unsigned int if using /dev/i2c-2 returned would be 2
 */
unsigned int maa_check_i2c();

/** Check spi interface, sets up multiplexer on device.
 *
 * @return unsigned int if using /dev/spi-1.0 returned would be 1.0
 */
double maa_check_spi();

/** Check PWM
 *
 * Will check input is valid for pwm and will also setup required multiplexers.
 * IF the pin also does gpio (strong chance), DO NOTHING, REV D is strange.
 * @param pin the pin as read from the board surface.
 * @return the pwm pin_info_t of that IO pin
 */
maa_pin_t* maa_check_pwm(int pin);

/** Get the version string of maa autogenerated from git tag
 *
 * The version returned may not be what is expected however it is a reliable
 * number associated with the git tag closest to that version at build time
 * @return version string from version.h
 */
const char* maa_get_version();

/** Print a textual representation of the maa_result_t
 *
 * @param result the result to print,
 */
void maa_result_print(maa_result_t result);

/** Checks if a pin is able to use the passed in mode.
 *
 * @param pin Physical Pin to be checked.
 * @param mode the mode to be tested.
 * @return boolean if the mode is supported, 0=false.
 */
maa_boolean_t maa_pin_mode_test(int pin, maa_pinmodes_t mode);

#ifdef __cplusplus
}
#endif
