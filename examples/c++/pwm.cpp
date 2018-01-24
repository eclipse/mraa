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
 *
 * Example usage: Generates PWM at a step rate of 0.01 continuously.
 *                Press Ctrl+C to exit
 */

/* standard headers */
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/pwm.hpp"

#define PWM_PORT 3

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        std::cout << "Exiting..." << std::endl;
        flag = 0;
    }
}

int
main(void)
{
    float value = 0.0f;

    signal(SIGINT, sig_handler);

    //! [Interesting]
    mraa::Pwm pwm(PWM_PORT);
    std::cout << "Cycling PWM on IO3 (pwm3)" << std::endl;
    pwm.enable(true);

    while (flag) {
        value = value + 0.01f;
        pwm.write(value);
        usleep(50000);
        if (value >= 1.0f) {
            value = 0.0f;
        }
    }
    //! [Interesting]

    return EXIT_SUCCESS;
}
