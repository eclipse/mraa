#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko.
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

MRAA_AIO_TEST_PIN = 0
PLATFORM_STD_ADC_RES_BITS = 10
PLATFORM_MAX_ADC_RES_BITS = 12

class AioChecks(u.TestCase):
  def setUp(self):
    self.pin = m.Aio(MRAA_AIO_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_aio_get_bit(self):
    self.assertEqual(self.pin.getBit(), PLATFORM_STD_ADC_RES_BITS, "Wrong ADC resolution reported")

  def test_aio_set_bit(self):
    self.pin.setBit(PLATFORM_MAX_ADC_RES_BITS)
    self.assertEqual(self.pin.getBit(), PLATFORM_MAX_ADC_RES_BITS, "Wrong ADC resolution reported after setBit()")

  def test_aio_read(self):
    self.assertNotEqual(self.pin.read(), -1, "Error returned when reading ADC value")

  def test_aio_read_float_std_res(self):
    self.pin.setBit(PLATFORM_STD_ADC_RES_BITS)
    self.assertNotEqual(self.pin.readFloat(), -1, "Error returned when reading float ADC value at standard resolution")

  def test_aio_read_float_max_res(self):
    self.pin.setBit(PLATFORM_MAX_ADC_RES_BITS)
    self.assertNotEqual(self.pin.readFloat(), -1, "Error returned when reading float ADC value at maximum resolution")

if __name__ == "__main__":
  u.main()
