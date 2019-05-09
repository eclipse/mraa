/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Sends data continuously to a Spi device. Press Ctrl+C to exit
 *
 */

/* standard headers */
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/spi.hpp"

#define SPI_PORT 0

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
    uint8_t data[] = { 0x00, 100 };
    uint8_t rxBuf[2];
    uint8_t* recv;
    int i;

    signal(SIGINT, sig_handler);

    //! [Interesting]
    mraa::Spi spi(SPI_PORT);

    while (flag) {
        for (i = 90; i < 130; i++) {
            data[1] = i;
            recv = spi.write(data, 2);
            std::cout << "Writing -%i" << i << std::endl;
            if (recv) {
                std::cout << "RECIVED-%i-%i" << recv[0] << recv[1] << std::endl;
                free(recv);
            }
            usleep(100000);
        }

        for (i = 130; i > 90; i--) {
            data[1] = i;
            if (spi.transfer(data, rxBuf, 2) == mraa::SUCCESS) {
                std::cout << "Writing -%i" << i << std::endl;
                std::cout << "RECIVED-%i-%i" << rxBuf[0] << rxBuf[1] << std::endl;
            }
            usleep(100000);
        }
    }
    //! [Interesting]

    return EXIT_SUCCESS;
}
