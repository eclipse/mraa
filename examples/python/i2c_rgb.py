#!/usr/bin/env python

# Author: Brendan Le Foll <brendan.le.foll@intel.com>
# Copyright (c) 2014 Intel Corporation.
#
# SPDX-License-Identifier: MIT
#
# Example Usage: Changes the Grove-LCD RGB backlight to a nice shade of purple

import mraa

# initialise I2C
x = mraa.I2c(0)
x.address(0x62)

# initialise device
x.writeReg(0, 0)
x.writeReg(1, 0)

# write RGB color data
x.writeReg(0x08, 0xAA)
x.writeReg(0x04, 255)
x.writeReg(0x02, 255)
