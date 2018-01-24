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
 * Example usage: Toggles GPIO's 23 and 24 recursively. Press Ctrl+C to exit
 *
 */

/* standard headers */
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/gpio.hpp"

/* gpio declaration */
#define GPIO_PIN_1 23
#define GPIO_PIN_2 24

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
    mraa::Result status;

    /* install signal handler */
    signal(SIGINT, sig_handler);

    //! [Interesting]
    /* initialize GPIO pin */
    mraa::Gpio gpio_1(GPIO_PIN_1);

    /* initialize GPIO pin */
    mraa::Gpio gpio_2(GPIO_PIN_2);

    /* set GPIO to output */
    status = gpio_1.dir(mraa::DIR_OUT);
    if (status != mraa::SUCCESS) {
        printError(status);
        return EXIT_FAILURE;
    }

    /* set gpio 24 to output */
    status = gpio_2.dir(mraa::DIR_OUT);
    if (status != mraa::SUCCESS) {
        printError(status);
        return EXIT_FAILURE;
    }

    /* toggle both GPIO's */
    while (flag) {
        status = gpio_1.write(1);
        if (status != mraa::SUCCESS) {
            printError(status);
            return EXIT_FAILURE;
        }
        status = gpio_2.write(0);
        if (status != mraa::SUCCESS) {
            printError(status);
            return EXIT_FAILURE;
        }

        sleep(1);

        status = gpio_1.write(0);
        if (status != mraa::SUCCESS) {
            printError(status);
            return EXIT_FAILURE;
        }

        status = gpio_2.write(1);
        if (status != mraa::SUCCESS) {
            printError(status);
            return EXIT_FAILURE;
        }

        sleep(1);
    }
    //! [Interesting]

    return EXIT_SUCCESS;
}
