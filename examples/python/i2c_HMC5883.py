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

import mraa

HMC5883L_CONF_REG_A = 0x00
HMC5883L_CONF_REG_B =  0x01
HMC5883L_CONT_MODE = 0x00
HMC5883L_DATA_REG = 0x03
GA_1_3_REG = 0x01

x = mraa.I2c(0)
x.address(0x1E)
x.writeReg(HMC5883L_CONF_REG_B, GA_1_3_REG)
x.writeReg(HMC5883L_CONF_REG_B, HMC5883L_CONT_MODE)

x.address(0x1E)
x.write(HMC5883L_DATA_REG)

# this method takes a char* as an input where sizeof(m) defines the read size
x.address(0x1E)
m = "      "
print (x.read(m))
print (m)

x.address(0x1E)
x.write(HMC5883L_DATA_REG)

# this method simply takes a size_t arg and returns an std::string, however
# it's less efficient
x.address(0x1E)
print (x.read(6))
