/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2018 Linaro Ltd.
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
 * Example usage: Configures GPIO pin for interrupt and waits 30 seconds for the isr to trigger
 *
 */

/* standard headers */
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/gpio.hpp"

#define GPIO_PIN 6

void
int_handler(void* args)
{
    std::cout << "ISR triggered" << std::endl;
}

int
main(void)
{
    mraa::Result status;

    //! [Interesting]
    /* initialize GPIO */
    mraa::Gpio gpio(GPIO_PIN);

    /* set GPIO to input */
    status = gpio.dir(mraa::DIR_IN);
    if (status != mraa::SUCCESS) {
        printError(status);
        return EXIT_FAILURE;
    }

    /* configure ISR for GPIO */
    status = gpio.isr(mraa::EDGE_BOTH, &int_handler, NULL);
    if (status != mraa::SUCCESS) {
        printError(status);
        return EXIT_FAILURE;
    }

    /* wait 30 seconds isr trigger */
    sleep(30);
    //! [Interesting]

    return EXIT_SUCCESS;
}
