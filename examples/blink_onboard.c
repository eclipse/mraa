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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mraa/gpio.h"

int
main(int argc, char **argv)
{
    mraa_platform_t platform = mraa_get_platform_type();
    mraa_gpio_context gpio, gpio_in = NULL;
    char board_name[] = "Some weird devboard that isn't recognised...";
    int ledstate = 0;

    switch (platform) {
        case MRAA_INTEL_GALILEO_GEN1:
            strcpy(board_name, "Intel Galileo Gen1");
            gpio = mraa_gpio_init_raw(3);
            break;
        case MRAA_INTEL_GALILEO_GEN2:
            strcpy(board_name, "Intel Galileo Gen2");
            gpio = mraa_gpio_init(13);
            break;
        case MRAA_INTEL_MINNOWBOARD_MAX:
            strcpy(board_name, "Intel Minnowboard Max");
            gpio = mraa_gpio_init(21);
            break;
        default:
            gpio = mraa_gpio_init(13);
    }

    fprintf(stdout, "Welcome to libmraa\n Version: %s\n Running on %s\n",
        mraa_get_version(), board_name);


    if (gpio == NULL) {
        fprintf(stdout, "Could not initilaize gpio\n");
        return 1;
    }

    if (platform == MRAA_INTEL_MINNOWBOARD_MAX) {
        gpio_in = mraa_gpio_init(14);
	if (gpio_in != NULL) {
            mraa_gpio_dir(gpio_in, MRAA_GPIO_IN);
            fprintf(stdout, "Press and hold S1 to stop\n");
        }
    }
    mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
    for (;;) {
        if (gpio_in != NULL && mraa_gpio_read(gpio_in) == 0) {
            return 0;
        }
        ledstate = !ledstate;
        mraa_gpio_write(gpio, !ledstate);
        sleep(1);
    }

    return 0;
}
