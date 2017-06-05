/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
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
#include "unistd.h"
#include <signal.h>

#include "mraa.h"

int running = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("closing down nicely\n");
        running = -1;
    }
}

int
main(int argc, char** argv)
{
    mraa_result_t r = MRAA_SUCCESS;

    mraa_init();
    fprintf(stdout, "MRAA Version: %s\nStarting Read on IO6 (Ctrl+C to exit)\n", mraa_get_version());

    //! [Interesting]
    mraa_gpio_context gpio;

    gpio = mraa_gpio_init(6);

    mraa_gpio_dir(gpio, MRAA_GPIO_IN);

    signal(SIGINT, sig_handler);

    while (running == 0) {
        fprintf(stdout, "Gpio is %d\n", mraa_gpio_read(gpio));
        sleep(1);
    }

    r = mraa_gpio_close(gpio);
    //! [Interesting]
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    return r;
}
