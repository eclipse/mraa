#!/usr/bin/env python

# Author: Brendan Le Foll <brendan.le.foll@intel.com>
# Copyright (c) 2014 Intel Corporation.
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

import mraa as m

# this example will show the 'advanced' i2c functionality from python i2c
# read/write

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
