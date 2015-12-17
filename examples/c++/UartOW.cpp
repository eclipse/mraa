/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2015 Intel Corporation
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

using namespace std;

int main(int argc, char** argv)
{
    mraa::UartOW *uart = new mraa::UartOW(0);

    // Reset the ow bus and see if anything is present
    mraa_uart_ow_status_t rv;

    if ((rv = uart->reset()) == MRAA_UART_OW_SUCCESS)
      {
        cout << "Reset suceeded, device(s) detected!" << endl;
      }
    else
      {
        cout << "Reset failed, returned " << int(rv) << ". No devices on bus?" 
             << endl;
        return 1;
      }
    
    cout << "Looking for devices..." << endl;;
    uint8_t id[MRAA_UART_OW_ROMCODE_SIZE];
    
    /* we are essentially doing a binary tree search through the 64
     * bit address space.  id is modified during this search, and will
     * be set to the valid rom code for each device found.
     */

    uint8_t count = 0;
    /* start at the beginning */
    uint8_t start = MRAA_UART_OW_SEARCH_ROM_FIRST;
    
    while (start != MRAA_UART_OW_SEARCH_ROM_LAST)
      {
        start = uart->search(start, id);
        if (start == MRAA_UART_OW_NO_DEVICES)
          {
            cout << "No devices detected." << endl;
            return 1;
          }
        if (start == MRAA_UART_OW_DATA_ERROR)
          {
            cout << "Bus/Data error." << endl;
            return 1;
          }
        
        /* The first byte (id[0]]) is the device type (family) code.
         * The last byte (id[7]) is the rom code CRC value.  The
         * intervening bytes (id[1]-id[6]) are the unique 48 bit
         * device ID.
         */
        printf("Device %03d Type 0x%02x ID %02hx%02hx%02hx%02hx%02hx%02hx CRC 0x%02hx\n",
               count, id[0], id[6], id[5], id[4], id[3], id[2], id[1], id[7]);
        count++;
      }
    
    cout << "Exiting..." << endl;

    delete uart;

    return 0;
}
//! [Interesting]
