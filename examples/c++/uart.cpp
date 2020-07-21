/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Prints "Hello Mraa!" recursively. Press Ctrl+C to exit
 *
 */

/* standard headers */
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/uart.hpp"

/* UART port */
#define UART_PORT 0

const char* dev_path = "/dev/ttyS0";

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
    signal(SIGINT, sig_handler);

    //! [Interesting]
    // If you have a valid platform configuration use numbers to represent uart
    // device. If not use raw mode where std::string is taken as a constructor
    // parameter
    mraa::Uart* uart, *temp;
    try {
        uart = new mraa::Uart(UART_PORT);
        temp = uart;
    } catch (std::exception& e) {
        std::cerr << e.what() << ", likely invalid platform config" << std::endl;
    }

    try {
        uart = new mraa::Uart(dev_path);
    } catch (std::exception& e) {
        std::cerr << "Error while setting up raw UART, do you have a uart?" << std::endl;
        std::terminate();
    }

    if (uart->setBaudRate(115200) != mraa::SUCCESS) {
        std::cerr << "Error setting parity on UART" << std::endl;
    }

    if (uart->setMode(8, mraa::UART_PARITY_NONE, 1) != mraa::SUCCESS) {
        std::cerr << "Error setting parity on UART" << std::endl;
    }

    if (uart->setFlowcontrol(false, false) != mraa::SUCCESS) {
        std::cerr << "Error setting flow control UART" << std::endl;
    }

    while (flag) {
        /* send data through uart */
        uart->writeStr("Hello Mraa!");

        sleep(1);
    }
    //! [Interesting]

    delete uart;
    delete temp;

    return EXIT_SUCCESS;
}
