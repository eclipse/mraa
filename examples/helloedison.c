/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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

#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <stdbool.h>
//! [Interesting]
#include "mraa.h"

int
main(int argc, char** argv)
{
    mraa_result_t ret = MRAA_SUCCESS;
    mraa_platform_t platform_type = mraa_get_platform_type();

    if (platform_type != MRAA_INTEL_EDISON_FAB_C) {
        fprintf(stderr, "Error: This program can only run on an edison\n");
        ret = MRAA_ERROR_INVALID_PLATFORM;
        goto end;
    }

    // MRAA_INTEL_EDISON_GP182 == 0 so this will initialise pin0 on arduino
    // which is hardware gpio 130 and not 182
    mraa_gpio_context gpio182 = mraa_gpio_init(MRAA_INTEL_EDISON_GP182);
    if (gpio182 == NULL) {
        fprintf(stderr, "Error: Failed to open gpio182\n");
        ret = MRAA_ERROR_INVALID_PLATFORM;
        goto end;
    }
    mraa_gpio_dir(gpio182, MRAA_GPIO_OUT);

    // we set the owner to false here, this makes sure that we do not close the
    // gpio from sysfs in mraa_gpio_close meaning it will stay as an output and
    // we will not always transition from 0->1 as gpio182 as output has the
    // default position of '0'. Note that the value could change as a result of
    // a mraa_gpio_dir however meaning we always go from 0->1 or 1->0
    mraa_gpio_owner(gpio182, false);
    int val = mraa_gpio_read(gpio182);
    printf("GPIO%d (mraa pin %d) was: %d, will set to %d\n", 182, mraa_gpio_get_pin(gpio182), val, !val);
    mraa_gpio_write(gpio182, !val);
    mraa_gpio_close(gpio182);

end:
    mraa_deinit();
    return ret;
}
//! [Interesting]
