#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from spi_checks_shared import *

class SpiChecksLsbmode(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_set_lsbmode_false(self):
    TEST_LSBMODE = False
    self.assertEqual(self.spi.lsbmode(TEST_LSBMODE),
                     m.SUCCESS,
                     "Setting LSB mode to %s did not return success" %TEST_LSBMODE)

  def test_spi_set_lsbmode_true(self):
    TEST_LSBMODE = True
    self.assertEqual(self.spi.lsbmode(TEST_LSBMODE),
                     m.SUCCESS,
                     "Setting LSB mode to %s did not return success" %TEST_LSBMODE)

  def test_spi_set_lsbmode_invalid(self):
    TEST_LSBMODE = 10
    self.assertRaises(TypeError, self.spi.lsbmode, TEST_LSBMODE)


if __name__ == "__main__":
  u.main()
