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

#pragma once

#include "common.h"
#include <string>

/**
 * @namespace mraa namespace
 */
namespace mraa
{

/**
 * @file
 * @brief API to common functions of MRAA
 *
 * This file defines the interface for libmraa common functions
 */

/**
 * Initialise MRAA
 *
 * Detects running platform and attempts to use included pinmap, this is run on
 * module/library init/load but is handy to rerun to check board initialised
 * correctly. MRAA_SUCCESS inidicates correct (first time) initialisation
 * whilst MRAA_ERROR_PLATFORM_ALREADY_INITIALISED indicates the board is
 * already initialised correctly
 *
 * @return Result of operation
 */
inline mraa_result_t
init()
{
    return mraa_init();
}

/**
 * Get libmraa version.
 *
 * @return libmraa version (e.g. v0.4.0-20-gb408207)
 */
inline std::string
getVersion()
{
    std::string ret = mraa_get_version();
    return ret;
}

/**
 * This function attempts to set the mraa process to a given priority and the
 * scheduler to SCHED_RR. Highest * priority is typically 99 and minimum is 0.
 * This function * will set to MAX if * priority is > MAX. Function will return
 * -1 on failure.
 *
 * @param priority Value from typically 0 to 99
 * @return The priority value set
 */
inline int
setPriority(const unsigned int priority)
{
    return mraa_set_priority(priority);
}

/**
 * Get platform type, board must be initialised.
 *
 * @return mraa_platform_t Platform type enum
 */
inline mraa_platform_t
getPlatformType()
{
    return mraa_get_platform_type();
}

/**
 * Print a textual representation of the mraa_result_t
 *
 * @param result the result to print
 */
inline void
printError(mraa_result_t result)
{
    mraa_result_print(result);
}

/**
 * Checks if a pin is able to use the passed in mode.
 *
 * @param pin Physical Pin to be checked.
 * @param mode the mode to be tested.
 * @return boolean if the mode is supported, 0=false.
 */
inline bool
pinModeTest(int pin, mraa_pinmodes_t mode)
{
    return (bool) mraa_pin_mode_test(pin, mode);
}

/**
 * Check the board's bit size when reading the value
 *
 * @return raw bits being read from kernel module. Zero if no ADC
 */
inline unsigned int
adcRawBits()
{
    return mraa_adc_raw_bits();
}

/**
 * Return value that the raw value should be shifted to. Zero if no ADC
 *
 * @return return actual bit size the adc value should be understood as.
 */
inline unsigned int
adcSupportedBits()
{
    return mraa_adc_supported_bits();
}

/**
 * Return Platform Name. Returns NULL if no platform inited.
 *
 * @return platform name
 */
inline std::string
getPlatformName()
{
    std::string ret_val(mraa_get_platform_name());
    return ret_val;
}

/**
 * Return count of physical pins on the running platform
 *
 * @return uint of physical pins.
 */
inline unsigned int
getPinCount()
{
    return mraa_get_pin_count();
}

/**
 * Get platform usable I2C bus count, board must be initialised.
 *
 * @return number f usable I2C bus count on the current platform. Function will
 * return -1 on failure
 */
inline int
getI2cBusCount()
{
    return mraa_get_i2c_bus_count();
}

/**
 * Get I2C adapter number in sysfs.
 *
 * @param i2c_bus the logical I2C bus number
 * @return I2C adapter number in sysfs. Function will return -1 on failure
 */
inline int
getI2cBusId(unsigned int i2c_bus)
{
    return mraa_get_i2c_bus_id(i2c_bus);
}

/**
* Get name of pin, board must be initialised.
*
* @param pin number
*
* @return char* of pin name
*/
inline std::string
getPinName(int pin)
{
    std::string ret_val(mraa_get_pin_name(pin));
    return ret_val;
}

/**
 * Sets the log level to use from 0-7 where 7 is very verbose. These are the
 * syslog log levels, see syslog(3) for more information on the levels.
 *
 * @param level
 * @return Result of operation
 */
inline mraa_result_t
setLogLevel(int level)
{
    return mraa_set_log_level(level);
}

/**
 * Detect presence of sub platform.
 *
 * @return bool true if sub platform is present and initialized, false otherwise
 */
inline bool
hasSubPlatform()
{
    return static_cast<bool>(mraa_has_sub_platform());
}



/**
 * Check if pin or bus id includes sub platform mask.
 *
 * @param int pin or bus number
 *
 * @return mraa_boolean_t 1 if pin or bus is for sub platform, 0 otherwise
 */
inline bool
isSubPlatformId(int pin_or_bus_id)
{
    return static_cast<bool>(mraa_is_sub_platform_id(pin_or_bus_id));
}

/**
 * Convert pin or bus index to corresponding sub platform id.
 *
 * @param int pin or bus index
 *
 * @return int sub platform pin or bus number
 */
inline int 
getSubPlatformId(int pin_or_bus_index)
{
    return mraa_get_sub_platform_id(pin_or_bus_index);
}

/**
 * Convert pin or bus sub platform id to index.
 *
 * @param int sub platform pin or bus id
 *
 * @return int pin or bus index
 */
int getSubPlatformIndex(int pin_or_bus_id)
{
    return mraa_get_sub_platform_index(pin_or_bus_id);
}
}
