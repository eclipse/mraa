#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2015 Alex Tereschenko
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
#
# Example Usage: Sends some info messages in the form of bytearray and strings
#                along with `X` flag and prints the response from other end.
#                This example requires uart_receiver.py to be running on the
#                other end.

import mraa
import sys

sys.stdout.write("Initializing UART...")
u = mraa.Uart(0)
print("...done")

print("Setting UART parameters: baudrate 115200, 8N1, no flow control")
u.setBaudRate(115200)
u.setMode(8, mraa.UART_PARITY_NONE, 1)
u.setFlowcontrol(False, False)

msg_b = bytearray("Hello, mraa byte array!", "ascii")
print("Sending message as a byte array: '{0}'".format(msg_b))
u.write(msg_b)
# Make sure the message gets out to the line.
# It's generally unnecessary (and performance-degrading) to do this explicitly,
# UART driver normally takes care of that, but it may be useful with specific
# half-duplex endpoints, like Dynamixel servos.
u.flush()

msg_s = "Hello, mraa string!"
print("Sending message as a string: '{0}'".format(msg_s))
u.writeStr(msg_s)

sys.stdout.write("Two-way, half-duplex communication, sending a flag...")
u.writeStr("X")
print("...sent, awaiting response...")
# Checking for data in the RX buffer, giving it a 100ms timeout
if u.dataAvailable(100):
    print("We've got a response: '{0}', says the other side".format(u.readStr(20)))
else:
    print("No data received, do you have anything at the other end?")
