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
//! [Interesting]
#include "mraa.h"

int
main(int argc, char** argv)
{
    mraa_uart_ow_context uart;
    uart = mraa_uart_ow_init(0);

    if (!uart) {
        printf("mraa_uart_ow_init() failed\n");
        return 1;
    }

    // Reset the ow bus and see if anything is present
    mraa_result_t rv;

    if ((rv = mraa_uart_ow_reset(uart)) == MRAA_SUCCESS) {
        printf("Reset succeeded, device(s) detected!\n");
    } else {
        printf("Reset failed, returned %d. No devices on bus?\n", rv);
        return 1;
    }

    printf("Looking for devices...\n");
    uint8_t id[MRAA_UART_OW_ROMCODE_SIZE];

    /* we are essentially doing a binary tree search through the 64
     * bit address space.  id is modified during this search, and will
     * be set to the valid rom code for each device found.
     */

    uint8_t count = 0;

    // start the search from scratch
    mraa_result_t result = mraa_uart_ow_rom_search(uart, 1, id);
    if (result == MRAA_ERROR_UART_OW_NO_DEVICES) {
        printf("No devices detected.\n");
        return 1;
    }

    if (result == MRAA_ERROR_UART_OW_DATA_ERROR) {
        printf("Bus/Data error.\n");
        return 1;
    }

    while (result == MRAA_SUCCESS) {
        /* The first byte (id[0]]) is the device type (family) code.
         * The last byte (id[7]) is the rom code CRC value.  The
         * intervening bytes (id[1]-id[6]) are the unique 48 bit
         * device ID.
         */
        printf("Device %02d Type 0x%02x ID %02x%02x%02x%02x%02x%02x CRC 0x%02x\n", count, id[0],
               id[6], id[5], id[4], id[3], id[2], id[1], id[7]);
        count++;

        // continue the search with start argument set to 0
        result = mraa_uart_ow_rom_search(uart, 0, id);
    }

    printf("Exiting...\n");

    mraa_uart_ow_stop(uart);

    return 0;
}
//! [Interesting]
