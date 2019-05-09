/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
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
