#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2015 Alex Tereschenko
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Expects 'X' flag from the sender. This example requires
#                `uart_sender.py` to be running on the other end.

import mraa

# Initialise UART
u = mraa.Uart(0)

# Set UART parameters
u.setBaudRate(115200)
u.setMode(8, mraa.UART_PARITY_NONE, 1)
u.setFlowcontrol(False, False)

# Start a neverending loop waiting for data to arrive.
# Press Ctrl+C to get out of it.
while True:
    if u.dataAvailable():
        # We are doing 1-byte reads here
        data_byte = u.readStr(1)
        print(data_byte)

        # Just a two-way half-duplex communication example, "X" is a flag
        if data_byte == "X":
            u.writeStr("Yes, master!")
