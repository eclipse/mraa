/*
 * Author: Sanrio Alvares <sanrio.alvares@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

#include "pwm.h"
#include "mraa_internal.h"
#include <string.h>

mraa_pwm_context
mraa_pwm_init(int pin)
{
    return NULL;
}

mraa_pwm_context
mraa_pwm_init_raw(int chipid, int pin)
{
    return NULL;
}

mraa_result_t
mraa_pwm_write(mraa_pwm_context dev, float percentage)
{
    return -1;
}

float
mraa_pwm_read(mraa_pwm_context dev)
{
    return -1;
}

mraa_result_t
mraa_pwm_period(mraa_pwm_context dev, float seconds)
{
    return -1;
}

mraa_result_t
mraa_pwm_period_ms(mraa_pwm_context dev, int ms)
{
    return -1;
}

mraa_result_t
mraa_pwm_period_us(mraa_pwm_context dev, int us)
{
    return -1;
}

mraa_result_t
mraa_pwm_pulsewidth(mraa_pwm_context dev, float seconds)
{
    return -1;
}

mraa_result_t
mraa_pwm_pulsewidth_ms(mraa_pwm_context dev, int ms)
{
    return -1;
}

mraa_result_t
mraa_pwm_pulsewidth_us(mraa_pwm_context dev, int us)
{
    return -1;
}

mraa_result_t
mraa_pwm_enable(mraa_pwm_context dev, int enable)
{
    return -1;
}

mraa_result_t
mraa_pwm_owner(mraa_pwm_context dev, mraa_boolean_t owner)
{
    return -1;
}

mraa_result_t
mraa_pwm_close(mraa_pwm_context dev)
{
    return -1;
}

int
mraa_pwm_get_max_period(mraa_pwm_context dev)
{
    return -1;
}

int
mraa_pwm_get_min_period(mraa_pwm_context dev)
{
    return -1;
}
