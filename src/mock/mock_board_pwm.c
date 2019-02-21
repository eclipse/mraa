/*
 * Author: Adelin Dobre <adelin.dobre@rinftech.com>
 * Copyright (c) 2019 Adelin Dobre.
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

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "common.h"
#include "mock/mock_board_pwm.h"

mraa_result_t
mraa_mock_pwm_init_raw_replace(mraa_pwm_context dev, int pin)
{
    if(dev->pin != pin) {
        syslog(LOG_ERR, "pwm_init: Failed to initialize the pin");
        return MRAA_ERROR_INVALID_PARAMETER;
    }
    dev->owner = 1;

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_pwm_write_period_replace(mraa_pwm_context dev, int period)
{
    dev->period = period;

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_pwm_write_duty_replace(mraa_pwm_context dev, int duty)
{
    dev->duty_fp = duty;

    return MRAA_SUCCESS;
}

int
mraa_mock_pwm_read_duty_replace(mraa_pwm_context dev)
{
    if(dev->duty_fp > INT_MAX || dev->duty_fp < INT_MIN) {
        syslog(LOG_ERR, "pwm%i read_duty: Number is invalid", dev->pin);
        return -1;
    }

    return dev->duty_fp;
}

mraa_result_t
mraa_mock_pwm_enable_replace(mraa_pwm_context dev, int enable)
{
    return MRAA_SUCCESS;
}

