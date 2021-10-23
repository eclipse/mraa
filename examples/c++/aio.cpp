/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Contributors: Alex Tereschenko <alex.mkrs@gmail.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
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
