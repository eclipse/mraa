#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
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
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import mraa as m
import unittest as u

from i2c_checks_shared import *

class I2cChecksReadByteData(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_read_byte_data(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    expected_res = MRAA_MOCK_I2C_DATA_INIT_BYTE
    res = self.i2c.readReg(MRAA_MOCK_I2C_DATA_LEN - 1)
    self.assertEqual(res, expected_res, "I2C readReg() returned unexpected data")

  def test_i2c_read_byte_data_invalid_addr(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR - 1)
    self.assertRaises(ValueError, self.i2c.readReg, MRAA_MOCK_I2C_DATA_LEN - 1)

  def test_i2c_read_byte_data_invalid_reg(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    self.assertRaises(ValueError, self.i2c.readReg, MRAA_MOCK_I2C_DATA_LEN)

if __name__ == "__main__":
  u.main()
