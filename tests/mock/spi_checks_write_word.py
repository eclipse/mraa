#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from spi_checks_shared import *

class SpiChecksWriteWord(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_write_word(self):
    TEST_WORD = 0xAAEE
    self.assertEqual(self.spi.writeWord(TEST_WORD),
                     TEST_WORD ^ MOCK_SPI_REPLY_DATA_MODIFIER_WORD,
                     "SPI writeWord() returned unexpected data")

  def test_spi_write_word_invalid_bigger_than_max(self):
    TEST_VALUE = 0xEEFFEE
    self.assertRaises(OverflowError, self.spi.writeWord, TEST_VALUE)

  def test_spi_write_word_invalid_smaller_than_min(self):
    TEST_VALUE = -1
    self.assertRaises(OverflowError, self.spi.writeWord, TEST_VALUE)

if __name__ == "__main__":
  u.main()
