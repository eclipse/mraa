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
 */

#include <unistd.h>
#include <signal.h>

#include "mraa.hpp"

int running = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("closing PWM nicely\n");
        running = -1;
    }
}

int
main()
{
    signal(SIGINT, sig_handler);
    //! [Interesting]
    mraa::Pwm* pwm;

    pwm = new mraa::Pwm(3);
    if (pwm == NULL) {
        return MRAA_ERROR_UNSPECIFIED;
    }
    fprintf(stdout, "Cycling PWM on IO3 (pwm3) \n");
    pwm->enable(true);

    float value = 0.0f;
    while (running == 0) {
        value = value + 0.01f;
        pwm->write(value);
        usleep(50000);
        if (value >= 1.0f) {
            value = 0.0f;
        }
    }
    delete pwm;
    //! [Interesting]

    return MRAA_SUCCESS;
}
