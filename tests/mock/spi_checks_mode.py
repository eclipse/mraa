#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

from spi_checks_shared import *

class SpiChecksMode(u.TestCase):
  def setUp(self):
    self.spi = m.Spi(MRAA_SPI_BUS_NUM)

  def tearDown(self):
    del self.spi

  def test_spi_set_mode_MODE0(self):
    TEST_MODE = m.SPI_MODE0
    self.assertEqual(self.spi.mode(TEST_MODE),
                     m.SUCCESS,
                     "Setting SPI mode to %d did not return success" %TEST_MODE)

  def test_spi_set_mode_MODE1(self):
    TEST_MODE = m.SPI_MODE1
    self.assertEqual(self.spi.mode(TEST_MODE),
                     m.SUCCESS,
                     "Setting SPI mode to %d did not return success" %TEST_MODE)

  def test_spi_set_mode_MODE2(self):
    TEST_MODE = m.SPI_MODE2
    self.assertEqual(self.spi.mode(TEST_MODE),
                     m.SUCCESS,
                     "Setting SPI mode to %d did not return success" %TEST_MODE)

  def test_spi_set_mode_MODE3(self):
    TEST_MODE = m.SPI_MODE3
    self.assertEqual(self.spi.mode(TEST_MODE),
                     m.SUCCESS,
                     "Setting SPI mode to %d did not return success" %TEST_MODE)

  def test_spi_set_mode_invalid_smaller_than_min(self):
    TEST_MODE = -10
    self.assertEqual(self.spi.mode(TEST_MODE),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting SPI mode to %d did not return error" %TEST_MODE)

  def test_spi_set_mode_invalid_bigger_than_max(self):
    TEST_MODE = 150
    self.assertEqual(self.spi.mode(TEST_MODE),
                     m.ERROR_INVALID_PARAMETER,
                     "Setting SPI mode to %d did not return error" %TEST_MODE)

if __name__ == "__main__":
  u.main()
