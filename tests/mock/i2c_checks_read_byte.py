#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from i2c_checks_shared import *

class I2cChecksReadByte(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_read_byte(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    expected_res = MRAA_MOCK_I2C_DATA_INIT_BYTE
    res = self.i2c.readByte()
    self.assertEqual(res, expected_res, "I2C readByte() returned unexpected data")

  def test_i2c_read_byte_invalid_addr(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR - 1)
    self.assertRaises(ValueError, self.i2c.readByte)

if __name__ == "__main__":
  u.main()
