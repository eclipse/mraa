/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2018 Linaro Ltd.
 *
 * SPDX-License-Identifier: MIT
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
