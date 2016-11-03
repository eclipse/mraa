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

class I2cChecksWriteWordData(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_write_word_data(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    high_byte = 0xAA
    low_byte = 0xBB
    test_word = (high_byte << 8) + low_byte
    reg = MRAA_MOCK_I2C_DATA_LEN - 2
    self.assertEqual(self.i2c.writeWordReg(reg, test_word),
                     m.SUCCESS,
                     "I2C writeWordReg() did not return success")
    self.assertEqual(self.i2c.readReg(reg),
                     high_byte,
                     "I2C readReg() of higher byte after writeWordReg() returned unexpected data")
    self.assertEqual(self.i2c.readReg(reg + 1),
                     low_byte,
                     "I2C readReg() of lower byte after writeWordReg() returned unexpected data")

  def test_i2c_write_word_data_invalid_addr(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR - 1)
    test_word = 0xAABB
    reg = MRAA_MOCK_I2C_DATA_LEN - 2
    self.assertEqual(self.i2c.writeWordReg(reg, test_word),
                     m.ERROR_UNSPECIFIED,
                     "I2C writeWordReg() to invalid address did not return error")

  def test_i2c_write_word_data_invalid_reg(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    test_word = 0xAABB
    reg = MRAA_MOCK_I2C_DATA_LEN
    self.assertEqual(self.i2c.writeWordReg(reg, test_word),
                     m.ERROR_UNSPECIFIED,
                     "I2C writeWordReg() with invalid register did not return error")

if __name__ == "__main__":
  u.main()
