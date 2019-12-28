#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from spi_checks_shared import *

class SpiChecksFreq(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_set_freq(self):
    TEST_FREQ = 20000000
    self.assertEqual(self.spi.frequency(TEST_FREQ),
                     m.SUCCESS,
                     "Setting SPI frequency to %d did not return success" %TEST_FREQ)

  def test_spi_set_freq_invalid_smaller_than_min_zero(self):
    TEST_FREQ = 0
    self.assertEqual(self.spi.frequency(TEST_FREQ),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting SPI frequency to %d did not return error" %TEST_FREQ)

  def test_spi_set_freq_invalid_smaller_than_min_negative(self):
    TEST_FREQ = -10
    self.assertEqual(self.spi.frequency(TEST_FREQ),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting SPI frequency to %d did not return error" %TEST_FREQ)

if __name__ == "__main__":
  u.main()
