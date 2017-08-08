/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2016 Intel Corporation
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

#include "stdio.h"
#include "iostream"
//! [Interesting]
#include "uart_ow.hpp"

int
main(int argc, char** argv)
{
    mraa::UartOW uart(0);

    // Reset the ow bus and see if anything is present
    mraa::Result rv;

    if ((rv = uart.reset()) == mraa::SUCCESS) {
        std::cout << "Reset succeeded, device(s) detected!" << std::endl;
    } else {
        std::cout << "Reset failed, returned " << int(rv) << ". No devices on bus?" << std::endl;
        return 1;
    }

    std::cout << "Looking for devices..." << std::endl;
    ;

    uint8_t count = 0;
    // start the search from scratch
    std::string id = uart.search(true);

    if (id.empty()) {
        std::cout << "No devices detected." << std::endl;
        return 1;
    }

    while (!id.empty()) {
        // hack so we don't need to cast each element of the romcode
        // for printf purposes
        uint8_t* ptr = (uint8_t*) id.data();

        // The first byte (0) is the device type (family) code.
        // The last byte (7) is the rom code CRC value.  The
        // intervening bytes are the unique 48 bit device ID.

        printf("Device %02d Type 0x%02x ID %02x%02x%02x%02x%02x%02x CRC 0x%02x\n", count, ptr[0],
               ptr[6], ptr[5], ptr[4], ptr[3], ptr[2], ptr[1], ptr[7]);
        count++;

        // continue the search with start argument set to false
        id = uart.search(false);
    }

    std::cout << "Exiting..." << std::endl;

    return 0;
}
//! [Interesting]
