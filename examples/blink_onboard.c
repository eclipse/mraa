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

#include "stdio.h"
#include <string.h>
#include "maa/gpio.h"

int
main(int argc, char **argv)
{
    maa_platform_t platform = maa_get_platform_type();
    maa_gpio_context gpio;
    char board_name[] = "Some weird devboard that isn't recognised...";
    int ledstate = 0;

    switch (platform) {
        case MAA_INTEL_GALILEO_GEN1:
            strcpy(board_name, "Intel Galileo Gen1");
            gpio = maa_gpio_init_raw(3);
            break;
        case MAA_INTEL_GALILEO_GEN2:
            strcpy(board_name, "Intel Galileo Gen2");
        default:
            gpio = maa_gpio_init(13);
    }

    fprintf(stdout, "Welcome to libmaa\n Version: %s\n Running on %d",
        maa_get_version(), board_name);

    maa_gpio_dir(gpio, MAA_GPIO_OUT);
    for (;;) {
        ledstate = !ledstate;
        maa_gpio_write(gpio, !ledstate);
        sleep(1);
    }

    return 0;
}
