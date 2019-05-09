#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from spi_checks_shared import *

class SpiChecksWrite(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_write(self):
    DATA_TO_WRITE = bytearray([0xEE for i in range(MOCK_SPI_TEST_DATA_LEN)])
    DATA_TO_EXPECT = bytearray([0xEE ^ MOCK_SPI_REPLY_DATA_MODIFIER_BYTE for i in range(MOCK_SPI_TEST_DATA_LEN)])
    self.assertEqual(self.spi.write(DATA_TO_WRITE),
                     DATA_TO_EXPECT,
                     "SPI write() returned unexpected data")

if __name__ == "__main__":
  u.main()
