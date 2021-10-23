#!/usr/bin/env python

# Author: Henry Bruce <henry.bruce@intel.com>
# Copyright (c) 2016 Intel Corporation.
#
# SPDX-License-Identifier: MIT

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
