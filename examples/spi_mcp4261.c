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

#include "mraa.h"
#include <unistd.h>
#include <stdint.h>

int
main(int argc, char** argv)
{
    mraa_init();
    //! [Interesting]
    mraa_spi_context spi;
    spi = mraa_spi_init(0);
    unsigned int response = 0;
    printf("Hello, SPI initialised\n");
    uint8_t data[] = { 0x00, 100 };
    uint8_t* recv;
    while (1) {
        int i;
        for (i = 90; i < 130; i++) {
            data[1] = i;
            recv = mraa_spi_write_buf(spi, data, 2);
            printf("Writing -%i", i);
            printf("RECIVED-%i-%i\n", recv[0], recv[1]);
            usleep(100000);
        }
        for (i = 130; i > 90; i--) {
            data[1] = i;
            recv = mraa_spi_write_buf(spi, data, 2);
            printf("Writing -%i", i);
            printf("RECIVED-%i-%i\n", recv[0], recv[1]);
            usleep(100000);
        }
    }
    //! [Interesting]
}
