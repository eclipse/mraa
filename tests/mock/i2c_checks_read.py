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

class I2cChecksRead(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_read_full_reg_range(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    expected_res = bytearray([MRAA_MOCK_I2C_DATA_INIT_BYTE for i in range(MRAA_MOCK_I2C_DATA_LEN)])
    res = self.i2c.read(MRAA_MOCK_I2C_DATA_LEN)
    self.assertEqual(res, expected_res, "I2C read() of full register range returned unexpected data")

  def test_i2c_read_part_reg_range(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    expected_res = bytearray([MRAA_MOCK_I2C_DATA_INIT_BYTE for i in range(MRAA_MOCK_I2C_DATA_LEN - 1)])
    res = self.i2c.read(MRAA_MOCK_I2C_DATA_LEN - 1)
    self.assertEqual(res, expected_res, "I2C read() of partial register range returned unexpected data")

  def test_i2c_read_invalid_addr(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR - 1)
    self.assertRaises(IOError, self.i2c.read, MRAA_MOCK_I2C_DATA_LEN)

  def test_i2c_read_invalid_len_bigger_than_max(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    self.assertRaises(IOError, self.i2c.read, MRAA_MOCK_I2C_DATA_LEN + 1)

if __name__ == "__main__":
  u.main()
