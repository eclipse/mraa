#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

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
