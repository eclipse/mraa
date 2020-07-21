#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from spi_checks_shared import *

class SpiChecksBitPerWord(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_bit_per_word(self):
    TEST_BIT_PER_WORD = 16
    self.assertEqual(self.spi.bitPerWord(TEST_BIT_PER_WORD),
                     m.SUCCESS,
                     "Setting bit per word to %d did not return success" %TEST_BIT_PER_WORD)

  def test_i2c_frequency_invalid_smaller_than_min(self):
    TEST_BIT_PER_WORD = -100
    self.assertRaises(OverflowError, self.spi.bitPerWord, TEST_BIT_PER_WORD)

if __name__ == "__main__":
  u.main()
