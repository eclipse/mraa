/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2014, 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
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