#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from spi_checks_shared import *

class SpiChecksWriteByte(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_write_byte(self):
    TEST_BYTE = 0xEE
    self.assertEqual(self.spi.writeByte(TEST_BYTE),
                     TEST_BYTE ^ MOCK_SPI_REPLY_DATA_MODIFIER_BYTE,
                     "SPI writeByte() returned unexpected data")

  def test_spi_write_byte_invalid_bigger_than_max(self):
    TEST_VALUE = 0xEEFF
    self.assertRaises(OverflowError, self.spi.writeByte, TEST_VALUE)

  def test_spi_write_byte_invalid_smaller_than_min(self):
    TEST_VALUE = -1
    self.assertRaises(OverflowError, self.spi.writeByte, TEST_VALUE)

if __name__ == "__main__":
  u.main()
