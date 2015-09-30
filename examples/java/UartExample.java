/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2014, 2015 Intel Corporation.
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


import mraa.Result;
import mraa.Uart;
import mraa.UartParity;

public class UartExample {

    public static void main(String[] args) {
        //! [Interesting]
        Uart uart = new Uart(0);

        if (uart.setBaudRate(115200) != Result.SUCCESS) {
            System.err.println("Error setting baud rate");
            System.exit(1);
        }

        if (uart.setMode(8, UartParity.UART_PARITY_NONE, 1) != Result.SUCCESS) {
            System.err.println("Error setting mode");
            System.exit(1);
        }

        if (uart.setFlowcontrol(false, false) != Result.SUCCESS) {
            System.err.println("Error setting flow control");
            System.exit(1);
        }

        uart.writeStr("Hello monkeys");
        //! [Interesting]
    }
}