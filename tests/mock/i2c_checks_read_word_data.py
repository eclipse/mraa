#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from i2c_checks_shared import *

class I2cChecksReadWordData(u.TestCase):
  def setUp(self):
    self.i2c = m.I2c(MRAA_I2C_BUS_NUM)

  def tearDown(self):
    del self.i2c

  def test_i2c_read_word_data(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    high_byte = 0xAA
    low_byte = 0xBB
    expected_res = (high_byte << 8) + low_byte
    self.i2c.writeReg(MRAA_MOCK_I2C_DATA_LEN - 2, high_byte)
    self.i2c.writeReg(MRAA_MOCK_I2C_DATA_LEN - 1, low_byte)
    res = self.i2c.readWordReg(MRAA_MOCK_I2C_DATA_LEN - 2)
    self.assertEqual(res, expected_res, "I2C readWordReg() returned unexpected data")

  def test_i2c_read_word_data_invalid_addr(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR - 1)
    self.assertRaises(ValueError, self.i2c.readWordReg, MRAA_MOCK_I2C_DATA_LEN - 2)

  def test_i2c_read_word_data_invalid_reg(self):
    self.i2c.address(MRAA_MOCK_I2C_ADDR)
    self.assertRaises(ValueError, self.i2c.readReg, MRAA_MOCK_I2C_DATA_LEN)

if __name__ == "__main__":
  u.main()
