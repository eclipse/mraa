#!/usr/bin/env python

# Author: Brendan Le Foll <brendan.le.foll@intel.com>
# Copyright (c) 2014 Intel Corporation.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Shows the 'advanced' i2c functionality from python i2c
#                read/write

import mraa as m

# initialise I2C
x = m.I2c(0)
x.address(0x77)

# initialise device
if x.readReg(0xd0) != 0x55:
    print("error")

# we want to read temperature so write 0x2e into control reg
x.writeReg(0xf4, 0x2e)

# read a 16bit reg, obviously it's uncalibrated so mostly a useless value :)
print(str(x.readWordReg(0xf6)))

# and we can do the same thing with the read()/write() calls if we wished
# thought I'd really not recommend it!

x.write(bytearray(b'0xf40x2e'))

x.writeByte(0xf6)
d = x.read(2)

# WARNING: python 3.2+ call
print(str(d))
print(int.from_bytes(d, byteorder='little'))
