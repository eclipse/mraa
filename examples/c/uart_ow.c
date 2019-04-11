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
 *
 * Example usage: Search and print the found OW device ID recursively.
 *
 */

/* standard headers */
#include "stdio.h"

/* mraa header */
#include "mraa.h"

int
main(void)
{
    mraa_uart_ow_context uart_ow;
    mraa_result_t status;
    uint8_t id[MRAA_UART_OW_ROMCODE_SIZE];
    uint8_t count = 0;

    //! [Interesting]
    uart_ow = mraa_uart_ow_init(0);
    if (uart_ow == NULL) {
        fprintf(stderr, "Failed to initialize UART OW\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    // Reset the ow bus and see if anything is present
    status = mraa_uart_ow_reset(uart_ow);
    if (status == MRAA_SUCCESS) {
        fprintf(stdout, "Reset succeeded, device(s) detected!\n");
    } else {
        goto err_exit;
    }

    fprintf(stdout, "Looking for devices...\n");

    /*
     * we are essentially doing a binary tree search through the 64
     * bit address space.  id is modified during this search, and will
     * be set to the valid rom code for each device found.
     *
     * start the search from scratch
     */
    status = mraa_uart_ow_rom_search(uart_ow, 1, id);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    while (status == MRAA_SUCCESS) {
        /* The first byte (id[0]]) is the device type (family) code.
         * The last byte (id[7]) is the rom code CRC value.  The
         * intervening bytes (id[1]-id[6]) are the unique 48 bit
         * device ID.
         */
        fprintf(stdout, "Device %02d Type 0x%02x ID %02x%02x%02x%02x%02x%02x CRC 0x%02x\n", count,
                id[0], id[6], id[5], id[4], id[3], id[2], id[1], id[7]);
        count++;

        // continue the search with start argument set to 0
        status = mraa_uart_ow_rom_search(uart_ow, 0, id);
    }

    /* stop uart_ow */
    mraa_uart_ow_stop(uart_ow);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the time) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* stop uart_ow */
    mraa_uart_ow_stop(uart_ow);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
