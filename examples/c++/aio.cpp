/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Contributors: Alex Tereschenko <alex.mkrs@gmail.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
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
 * Example usage: Reads ADC A0 value continuously. Press Ctrl+C to exit.
 */

/* standard headers */
#include <iostream>
#include <signal.h>
#include <stdlib.h>

/* mraa headers */
#include "mraa/aio.hpp"
#include "mraa/common.hpp"

/* AIO port */
#define AIO_PORT 0

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
    uint16_t value;
    float float_value;

    signal(SIGINT, sig_handler);

    //! [Interesting]
    /* initialize AIO */
    mraa::Aio aio(AIO_PORT);

    while (flag) {
        value = aio.read();
        float_value = aio.readFloat();
        std::cout << "ADC A0 read %X - %d" << value << value << std::endl;
        std::cout << "ADC A0 read float - %.5f" << float_value << std::endl;
    }
    //! [Interesting]

    return EXIT_SUCCESS;
}
