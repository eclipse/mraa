/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Contributors: Alex Tereschenko <alex.mkrs@gmail.com>
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

#include <signal.h>

#include "mraa.hpp"

int running = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("closing down nicely\n");
        running = -1;
    }
}

//! [Interesting]
int
main()
{
    uint16_t adc_value;
    float adc_value_float;
    mraa::Aio a0(0);

    signal(SIGINT, sig_handler);

    while (running == 0) {
        adc_value = a0.read();
        adc_value_float = a0.readFloat();
        fprintf(stdout, "ADC A0 read %X - %d\n", adc_value, adc_value);
        fprintf(stdout, "ADC A0 read float - %.5f (Ctrl+C to exit)\n", adc_value_float);
    }

    return MRAA_SUCCESS;
}
//! [Interesting]
