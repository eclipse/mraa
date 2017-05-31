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

class I2cChecksReadBytesData(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_read_bytes_data(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    # Generate unique data bytes
    data_to_write = bytearray([0xEE+i for i in range(MRAA_MOCK_I2C_DATA_LEN)])
    self.i2c.write(data_to_write)
    # We expect to read the last two bytes
    expected_res = bytearray(data_to_write[-2:])
    test_reg_addr = MRAA_MOCK_I2C_DATA_LEN - 2
    test_read_len = 2
    self.assertEqual(self.i2c.readBytesReg(test_reg_addr, test_read_len),
                     expected_res,
                     "I2C readBytesReg() returned unexpected data")

  def test_i2c_read_bytes_data_length_bigger_than_max(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    # Generate unique data bytes
    data_to_write = bytearray([0xEE+i for i in range(MRAA_MOCK_I2C_DATA_LEN)])
    self.i2c.write(data_to_write)
    # We expect to read the last two bytes
    expected_res = bytearray(data_to_write[-2:])
    test_reg_addr = MRAA_MOCK_I2C_DATA_LEN - 2
    # Set the read length bigger than our data length
    test_read_len = MRAA_MOCK_I2C_DATA_LEN + 2
    self.assertEqual(self.i2c.readBytesReg(test_reg_addr, test_read_len),
                     expected_res,
                     "I2C readBytesReg() returned unexpected data")

  def test_i2c_read_bytes_data_zero_length(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    self.assertRaises(IOError, self.i2c.readBytesReg, 0x0, 0)

  def test_i2c_read_bytes_data_negative_length(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    self.assertRaises(ValueError, self.i2c.readBytesReg, 0x0, -1)

  def test_i2c_read_bytes_data_invalid_addr(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR - 1)
    self.assertRaises(IOError, self.i2c.readBytesReg, 0x0, MRAA_MOCK_I2C_DATA_LEN - 1)

  def test_i2c_read_bytes_data_invalid_reg(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    self.assertRaises(IOError, self.i2c.readBytesReg, MRAA_MOCK_I2C_DATA_LEN, MRAA_MOCK_I2C_DATA_LEN - 1)

if __name__ == "__main__":
  u.main()
