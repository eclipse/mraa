/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
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

#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"

int running = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("closing spi nicely\n");
        running = -1;
    }
}

int
main()
{
    signal(SIGINT, sig_handler);

    //! [Interesting]
    mraa::Spi* spi;

    spi = new mraa::Spi(0);

    uint8_t data[] = { 0x00, 100 };
    uint8_t rxBuf[2];
    uint8_t* recv;
    while (running == 0) {
        int i;
        for (i = 90; i < 130; i++) {
            data[1] = i;
            recv = spi->write(data, 2);
            printf("Writing -%i", i);
            if (recv) {
                printf("RECIVED-%i-%i\n", recv[0], recv[1]);
                free(recv);
            }
            usleep(100000);
        }
        for (i = 130; i > 90; i--) {
            data[1] = i;
            if (spi->transfer(data, rxBuf, 2) == mraa::SUCCESS) {
                printf("Writing -%i", i);
                printf("RECIVED-%i-%i\n", rxBuf[0], rxBuf[1]);
            }
            usleep(100000);
        }
    }
    delete spi;
    //! [Interesting]

    return mraa::SUCCESS;
}
