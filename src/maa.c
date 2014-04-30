/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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

#include <stddef.h>

#include "maa.h"
#include "gpio.h"
#include "version.h"

static maa_pininfo* pindata;

const char *
maa_get_version()
{
    return gVERSION;
}

maa_result_t
maa_init()
{
    return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static maa_result_t
maa_setup_mux_mapped(maa_pininfo meta)
{
    int mi;
    for(mi = 0; mi < meta.mux_total; mi++) {
        maa_gpio_context* mux_i;
        mux_i = maa_gpio_init_raw(meta.mux[mi].pin);
        maa_gpio_dir(mux_i, MAA_GPIO_OUT);
        maa_gpio_write(mux_i, meta.mux[mi].value);
    }
}

unsigned int
maa_check_gpio(int pin){

    if(pindata == NULL) {
        return -1;
    }
    //Check in gpio bounds?
    if(pindata[pin].mux_total > 0)
        if(maa_setup_mux_mapped(pindata[pin]) != MAA_SUCCESS)
            return -1;

    return pindata[pin].pin;
}

