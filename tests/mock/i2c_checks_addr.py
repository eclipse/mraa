#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from i2c_checks_shared import *

class I2cChecksAddr(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_address(self):
    self.assertEqual(self.i2c.address(0x10),
                     m.SUCCESS,
                     "Setting address to 0x10 did not return success")

  def test_i2c_address_invalid_bigger_than_max(self):
    # For standard 7-bit addressing 0x7F is max address
    self.assertEqual(self.i2c.address(0xFF),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting address to 0xFF did not return INVALID_PARAMETER")

  def test_i2c_address_invalid_smaller_than_min(self):
    self.assertRaises(OverflowError, self.i2c.address, -100)

if __name__ == "__main__":
  u.main()
