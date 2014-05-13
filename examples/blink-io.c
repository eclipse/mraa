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
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "gpio.h"

#define DEFAULT_IOPIN 8

int running = 0;
static int iopin;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("closing IO%d nicely\n", iopin);
        running = -1;
    }
}

int
main(int argc, char **argv)
{
    maa_result_t r = MAA_SUCCESS;
    iopin = DEFAULT_IOPIN;

    if (argc < 2) {
        printf("Provide an int arg if you want to flash on something other than %d\n", DEFAULT_IOPIN);
    } else {
        iopin = strtol(argv[1], NULL, 10);
    }

    maa_init();
    fprintf(stdout, "MAA Version: %s\nStarting Blinking on IO%d\n",
            maa_get_version(), iopin);

    maa_gpio_context* gpio;
    gpio = maa_gpio_init(iopin);
    printf("Initialised pin%d which is atually pin%d\n", iopin, gpio->pin);

    // set direction to OUT
    r = maa_gpio_dir(gpio, MAA_GPIO_OUT);
    if (r != MAA_SUCCESS) {
        maa_result_print(r);
    }

    signal(SIGINT, sig_handler);

    while (running == 0) {
        r = maa_gpio_write(gpio, 0);
        if (r != MAA_SUCCESS) {
            maa_result_print(r);
        } else {
            printf("off\n");
        }

        sleep(1);

        r = maa_gpio_write(gpio, 1);
        if (r != MAA_SUCCESS) {
            maa_result_print(r);
        } else {
            printf("on\n");
        }

        sleep(1);
    }

    r = maa_gpio_close(gpio);
    if (r != MAA_SUCCESS) {
        maa_result_print(r);
    }

    return r;
}
