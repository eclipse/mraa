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

namespace mraa {

/**
 * @file
 * @brief C++ API to common functions of MRAA
 *
 * This file defines the C++ interface for libmraa common functions
 */

/**
 * Get libmraa version.
 *
 * @return libmraa version (e.g. v0.4.0-20-gb408207)
*/
std::string getVersion()
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
int setPriority(const unsigned int priority)
{
    return mraa_set_priority(priority);
}

/**
 * Get platform type, board must be initialised.
 *
 * @return mraa_platform_t Platform type enum
 */
mraa_platform_t getPlatformType()
{
    return mraa_get_platform_type();
}

/**
 * Print a textual representation of the mraa_result_t
 *
 * @param result the result to print
 */
void printError(mraa_result_t result)
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
bool pinModeTest(int pin, mraa_pinmodes_t mode)
{
    return (bool) mraa_pin_mode_test(pin,mode);
}

/**
 * Check the board's bit size when reading the value
 *
 * @return raw bits being read from kernel module. Zero if no ADC
 */
unsigned int adcRawBits()
{
    return mraa_adc_raw_bits();
}

/**
 * Return value that the raw value should be shifted to. Zero if no ADC
 *
 * @return return actual bit size the adc value should be understood as.
 */
unsigned int adcSupportedBits()
{
    return mraa_adc_supported_bits();
}

}
