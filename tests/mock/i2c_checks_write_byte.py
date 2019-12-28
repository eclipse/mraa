#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from i2c_checks_shared import *

class I2cChecksWriteByte(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_write_byte(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    test_byte = 0xEE
    self.assertEqual(self.i2c.writeByte(test_byte),
                     m.SUCCESS,
                     "I2C writeByte() did not return success")
    self.assertEqual(self.i2c.readByte(),
                     test_byte,
                     "I2C readByte() after writeByte() returned unexpected data")

  def test_i2c_write_byte_invalid_addr(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR - 1)
    self.assertEqual(self.i2c.writeByte(0xEE),
                     m.ERROR_UNSPECIFIED,
                     "I2C writeByte() to invalid address did not return error")

if __name__ == "__main__":
  u.main()
