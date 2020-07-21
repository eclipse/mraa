/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Search and print the found OW device ID recursively.
 *
 */

/* standard headers */
#include <iostream>
#include <stdlib.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/uart_ow.hpp"

int
main(void)
{
    mraa::Result status;
    std::string id;
    uint8_t count = 0;
    uint8_t* ptr;

    //! [Interesting]
    mraa::UartOW uart(0);

    // Reset the ow bus and see if anything is present
    if ((status = uart.reset()) == mraa::SUCCESS) {
        std::cout << "Reset succeeded, device(s) detected!" << std::endl;
    } else {
        std::cout << "Reset failed, returned " << int(status) << ". No devices on bus?" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Looking for devices..." << std::endl;

    // start the search from scratch
    id = uart.search(true);

    if (id.empty()) {
        std::cout << "No devices detected." << std::endl;
        return EXIT_FAILURE;
    }

    while (!id.empty()) {
        // hack so we don't need to cast each element of the romcode
        // for printf purposes
        ptr = (uint8_t*) id.data();

        // The first byte (0) is the device type (family) code.
        // The last byte (7) is the rom code CRC value.  The
        // intervening bytes are the unique 48 bit device ID.

        std::cout << "Device %02d Type 0x%02x ID %02x%02x%02x%02x%02x%02x CRC 0x%02x" << count << ptr[0]
                  << ptr[6] << ptr[5] << ptr[4] << ptr[3] << ptr[2] << ptr[1] << ptr[7] << std::endl;
        count++;

        // continue the search with start argument set to false
        id = uart.search(false);
    }

    std::cout << "Exiting..." << std::endl;
    //! [Interesting]

    return EXIT_SUCCESS;
}
