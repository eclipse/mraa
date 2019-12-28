/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2018 Linaro Ltd.
 *
 * SPDX-License-Identifier: MIT
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
