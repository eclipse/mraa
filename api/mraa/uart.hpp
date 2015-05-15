/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
        return;
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

  private:
    mraa_uart_context m_uart;
};
}
