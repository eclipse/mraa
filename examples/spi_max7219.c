/*
 * Author: Michael Ring <mail@michael-ring.org>
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
    //! [Interesting]
    mraa_spi_context spi;
    spi = mraa_spi_init(1);
    if (spi == NULL) {
        printf("Initialization of spi failed, check syslog for details, exit...\n");
        exit(1);
    }

    printf("SPI initialised successfully\n");

    mraa_spi_frequency(spi, 400000);
    mraa_spi_lsbmode(spi, 0);

    // The MAX7219/21 Chip needs the data in word size
    if (mraa_spi_bit_per_word(spi, 16) != MRAA_SUCCESS) {
        printf("Could not set SPI Device to 16Bit mode, exit...\n");
        exit(1);
    };

    mraa_spi_write_word(spi, 0x0900); // Do not decode bits
    mraa_spi_write_word(spi, 0x0a05); // Brightness of LEDs
    mraa_spi_write_word(spi, 0x0b07); // Show all Scan Lines
    mraa_spi_write_word(spi, 0x0c01); // Display on
    mraa_spi_write_word(spi, 0x0f00); // Testmode off

    // Display Pattern on the display
    uint16_t dataAA55[] = { 0x01aa, 0x0255, 0x03aa, 0x0455, 0x05aa, 0x0655, 0x07aa, 0x0855 };
    mraa_spi_write_buf_word(spi, dataAA55, 16);

    sleep(2);

    // Display inverted Pattern
    uint16_t data55AA[] = { 0x0155, 0x02aa, 0x0355, 0x04aa, 0x0555, 0x06aa, 0x0755, 0x08aa };
    mraa_spi_write_buf_word(spi, data55AA, 16);

    sleep(2);

    // Clear the display
    uint16_t data[] = { 0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700, 0x0800 };
    mraa_spi_write_buf_word(spi, data, 16);

    int i;
    int j;
    // cycle through all LED's
    for (i = 1; i <= 8; i++) {
        for (j = 0; j < 8; j++) {
            mraa_spi_write_word(spi, (i << 8) + (1 << j));
            sleep(1);
        }
        mraa_spi_write_word(spi, i << 8);
    }

    mraa_spi_stop(spi);

    //! [Interesting]
}
