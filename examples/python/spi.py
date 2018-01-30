#!/usr/bin/env python

# Author: Henry Bruce <henry.bruce@intel.com>
# Copyright (c) 2016 Intel Corporation.
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

# Example Usage: Read from MCP3004 ADC pin 0 in single ended mode

import mraa
import time

# initialise SPI
dev = mraa.Spi(0)

# prepare data to send
txbuf = bytearray(3)
txbuf[0] = 0x01
txbuf[1] = 0x80
txbuf[2] = 0x00

while True:
    # send data through SPI
    rxbuf = dev.write(txbuf)
    value = ((rxbuf[1] & 0x03) << 8) | rxbuf[2]
    print(value)

    time.sleep(0.5)
