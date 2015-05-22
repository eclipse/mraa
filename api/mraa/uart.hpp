/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Contributions: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2014 - 2015 Intel Corporation.
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

#pragma once

#include "uart.h"
#include <stdexcept>

namespace mraa
{

/**
 * @brief API to UART (enabling only)
 *
 * This file defines the UART interface for libmraa
 */
class Uart
{
  public:
    /**
     * Uart Constructor, takes a pin number which will map directly to the
     * linux uart number, this 'enables' the uart, nothing more
     *
     * @param uart the index of the uart set to use
     */
    Uart(int uart)
    {
        m_uart = mraa_uart_init(uart);

        if (m_uart == NULL) {
            throw std::invalid_argument("Error initialising UART");
        }
    }
    /**
     * Uart destructor
     */
    ~Uart()
    {
        mraa_uart_stop(m_uart);
    }

    /**
     * Get string with tty device path within Linux
     * For example. Could point to "/dev/ttyS0"
     *
     * @return char pointer of device path
     */
    std::string
    getDevicePath()
    {
        std::string ret_val(mraa_uart_get_dev_path(m_uart));
        return ret_val;
    }

    /**
     * Read bytes from the device into a buffer
     *
     * @param data buffer pointer
     * @param length maximum size of buffer
     * @return the number of bytes read, or -1 if an error occurred
     */
    int
    read(char* data, int length)
    {
        return mraa_uart_read(m_uart, data, length);
    }

    /**
     * Write bytes in buffer to a device
     *
     * @param data buffer pointer
     * @param length maximum size of buffer
     * @return the number of bytes written, or -1 if an error occurred
     */
    int
    write(char* data, int length)
    {
        return mraa_uart_write(m_uart, data, length);
    }

    /**
     * Check to see if data is available on the device for reading
     *
     * @param millis number of milliseconds to wait, or 0 to return immediately
     * @return true if there is data available to read, false otherwise
     */
    bool
    dataAvailable(unsigned int millis = 0)
    {
        if (mraa_uart_data_available(m_uart, millis))
            return true;
        else
            return false;
    }

  private:
    mraa_uart_context m_uart;
};
}
