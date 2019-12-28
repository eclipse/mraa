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
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>
#include "types.h"

/** Max size off Mraa Platform name */
#define MRAA_PLATFORM_NAME_MAX_SIZE 64
/** Size off Mraa pin name */
#define MRAA_PIN_NAME_SIZE 12

/** Bit Shift for Mraa sub platform */
#define MRAA_SUB_PLATFORM_BIT_SHIFT 9
/** Mask for Mraa sub platform */
#define MRAA_SUB_PLATFORM_MASK (1<<MRAA_SUB_PLATFORM_BIT_SHIFT)

/** Mraa main platform offset */
#define MRAA_MAIN_PLATFORM_OFFSET 0
/** Mraa sub platform offset */
#define MRAA_SUB_PLATFORM_OFFSET 1

/** Executes function func and returns its result in case of error
 */
#define MRAA_RETURN_FOR_ERROR(func) do { \
                                      mraa_result_t res; \
                                      res = func; \
                                      if (res != MRAA_SUCCESS) \
                                      return res;} while(0)

/**
 * Simple deprecated macro
 */
#ifdef __GNUC__
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

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
 * Initialise MRAA
 *
 * Detects running platform and attempts to use included pinmap, this is run on
 * module/library init/load but is handy to rerun to check board initialised
 * correctly. MRAA_SUCCESS inidicates correct initialisation.
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
 * people who like super clean code. If dynamically loading & unloading
 * libmraa you need to call this before unloading the library.
 */
void mraa_deinit() __attribute__((destructor));

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
 * Check the specified board's bit size when reading the value
 *
 * @param platform_offset specified platform offset; 0 for main platform, 1 for sub platform
 * @return raw bits being read from kernel module. zero if no ADC
 */
unsigned int mraa_get_platform_adc_raw_bits(uint8_t platform_offset);

/**
 * Return value that the raw value should be shifted to. Zero if no ADC
 *
 * @return return actual bit size the adc value should be understood as.
 */
unsigned int mraa_adc_supported_bits();

/**
 * Return value that the raw value should be shifted to. Zero if no ADC
 *
 * @param platform_offset specified platform offset; 0 for main platform, 1 for sub platform
 * @return return actual bit size the adc value should be understood as.
 */
unsigned int mraa_get_platform_adc_supported_bits(int platform_offset);

/**
 * Sets the log level to use from 0-7 where 7 is very verbose. These are the
 * syslog log levels, see syslog(3) for more information on the levels.
 *
 * @return Result of operation
 */
mraa_result_t mraa_set_log_level(int level);

/**
 * Return the Platform's Name, If no platform detected return NULL
 *
 * @return platform name
 */
const char* mraa_get_platform_name();

/**
 * Return the platform's versioning info, the information given depends per
 * platform and can be NULL. platform_offset has to be given. Do not modify
 * this pointer
 *
 * @param platform_offset specified platform offset; 0 for main platform, 1 for sub platform
 * @return platform's versioning string
 */
const char* mraa_get_platform_version(int platform_offset);

/**
 * This function attempts to set the mraa process to a given priority and the
 * scheduler to SCHED_RR. Highest * priority is typically 99 and minimum is 0.
 * This function * will set to MAX if * priority is > MAX. Function will return
 * -1 on failure.
 *
 * @param priority Value from typically 0 to 99
 * @return The priority value set
 */
int mraa_set_priority(const int priority);

/** Get the version string of mraa autogenerated from git tag
 *
 * The version returned may not be what is expected however it is a reliable
 * number associated with the git tag closest to that version at build time
 *
 * @return version string from version.h
 */
const char* mraa_get_version();

/**
 * Print a textual representation of the mraa_result_t
 *
 * @param result the result to print
 */
void mraa_result_print(mraa_result_t result);

/**
 * Get platform type, board must be initialised.
 *
 * @return mraa_platform_t Platform type enum
 */
mraa_platform_t mraa_get_platform_type();

/**
 * Get combined platform type, board must be initialised.
 * The combined type is represented as
 * (sub_platform_type << 8) | main_platform_type
 *
 * @return int combined platform type
 */
int mraa_get_platform_combined_type();

/**
 * Get platform pincount, board must be initialised.
 *
 * @return uint of physical pin count on the in-use platform
 */
unsigned int mraa_get_pin_count();

/**
 * Get the number of usable UARTs, board must be initialised.
 *
 * @return number of usable UARTs on the platform, returns -1 on failure.
 */
int mraa_get_uart_count();

/**
 * Get the number of usable SPI buses, board must be initialised.
 *
 * @return number of usable SPI buses on the platform, returns -1 on failure.
 */
int mraa_get_spi_bus_count();

/**
 * Get the number of usable PWM pins, board must be initialised.
 *
 * @return number of PWMs on the current platform, -1 on failure.
 */
int mraa_get_pwm_count();

/**
 * Get the number of usable GPIOs, board must be initialised.
 *
 * @return number of usable external GPIO pins on the board, -1 on failure.
 */
int mraa_get_gpio_count();

/**
 * Get the number of usable analog pins, board must be initialised.
 *
 * @return number of usable ADC inputs on the platform and -1 on failure.
 */
int mraa_get_aio_count();

/**
 * Get platform usable I2C bus count, board must be initialised.
 *
 * @return number f usable I2C bus count on the current platform. Function will
 * return -1 on failure
 */
int mraa_get_i2c_bus_count();

/**
 * Get I2C adapter number in sysfs.
 *
 * @param i2c_bus the logical I2C bus number
 * @return I2C adapter number in sysfs. Function will return -1 on failure
 */
int mraa_get_i2c_bus_id(int i2c_bus);

/**
 * Get specified platform pincount, board must be initialised.
 *
 * @param platform_offset specified platform offset; 0 for main platform, 1 for sub platform
 * @return uint of physical pin count on the in-use platform
 */
unsigned int mraa_get_platform_pin_count(uint8_t platform_offset);

/**
* Get name of pin, board must be initialised.
*
* @param pin number
* @return char* of pin name
*/
char* mraa_get_pin_name(int pin);

/**
* Get GPIO index by pin name, board must be initialised.
*
* @param pin_name: GPIO pin name. Eg: IO0
* @return int of MRAA index for GPIO or -1 if not found.
*/
int mraa_gpio_lookup(const char* pin_name);

/**
* Get I2C bus index by bus name, board must be initialised.
*
* @param i2c_name: I2C bus name. Eg: I2C6
* @return int of MRAA index for I2C bus or -1 if not found.
*/
int mraa_i2c_lookup(const char* i2c_name);

/**
* Get SPI bus index by bus name, board must be initialised.
*
* @param spi_name: Name of SPI bus. Eg: SPI2
* @return int of MRAA index for SPI bus or -1 if not found.
*/
int mraa_spi_lookup(const char* spi_name);

/**
 * Get PWM index by PWM name, board must be initialised.
 *
 * @param pwm_name: Name of PWM. Eg:PWM0
 * @return int of MRAA index for PWM or -1 if not found.
 */
int mraa_pwm_lookup(const char* pwm_name);

/**
 * Get UART index by name, board must be initialised.
 *
 * @param uart_name: Name of UART. Eg:UART1
 * @return int of MRAA index for UART, or -1 if not found.
 */
int mraa_uart_lookup(const char* uart_name);

/**
 * Get default i2c bus, board must be initialised.
 *
 * @return int default i2c bus index
 */
int mraa_get_default_i2c_bus(uint8_t platform_offset);

/**
 * Detect presence of sub platform.
 *
 * @return mraa_boolean_t 1 if sub platform is present and initialized, 0 otherwise
 */
mraa_boolean_t mraa_has_sub_platform();


/**
 * Check if pin or bus id includes sub platform mask.
 *
 * @param pin_or_bus_id pin or bus number
 *
 * @return mraa_boolean_t 1 if pin or bus is for sub platform, 0 otherwise
 */
mraa_boolean_t mraa_is_sub_platform_id(int pin_or_bus_id);

/**
 * Convert pin or bus index to corresponding sub platform id.
 *
 * @param pin_or_bus_index pin or bus index
 *
 * @return int sub platform pin or bus number
 */
int mraa_get_sub_platform_id(int pin_or_bus_index);

/**
 * Convert pin or bus sub platform id to index.
 *
 * @param pin_or_bus_id sub platform pin or bus id
 *
 * @return int pin or bus index
 */
int mraa_get_sub_platform_index(int pin_or_bus_id);

/**
 * Add mraa subplatform
 *
 * @param subplatformtype subplatform type
 * @param dev uart device or i2c bus subplatform is on
 *
 * @return mraa_result_t indicating success
 */
mraa_result_t mraa_add_subplatform(mraa_platform_t subplatformtype, const char* dev);

/**
 * Remove a mraa subplatform
 *
 * @param subplatformtype subplatform type
 *
 * @return mraa_result indicating success
 */
mraa_result_t mraa_remove_subplatform(mraa_platform_t subplatformtype);

/**
 * Create IO using a description in the format:
 * [io]-[pin]
 * [io]-[raw]-[pin]
 * [io]-[raw]-[id]-[pin]
 * [io]-[raw]-[path]
 *
 * @param desc IO description
 *
 * @return void* to IO context or NULL
 */
void* mraa_init_io(const char* desc);

/**
 * Instantiate an unknown board using a json file
 *
 * @param path Path to the json file, relative to the folder the program
 * was initially run in or a direct path
 *
 * @return mraa_result indicating success
 */
mraa_result_t mraa_init_json_platform(const char* path);

#ifdef __cplusplus
}
#endif
