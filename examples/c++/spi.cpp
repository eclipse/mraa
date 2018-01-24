/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
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
