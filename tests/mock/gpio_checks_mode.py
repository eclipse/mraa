#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko <alext.mkrs@gmail.com>
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

MRAA_TEST_PIN = 0

class GpioChecksMode(u.TestCase):
  def setUp(self):
    self.pin = m.Gpio(MRAA_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_set_mode_STRONG(self):
      res = self.pin.mode(m.MODE_STRONG)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting GPIO mode to MODE_STRONG did not return unimplemented")

  def test_set_mode_PULLUP(self):
      res = self.pin.mode(m.MODE_PULLUP)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting GPIO mode to MODE_PULLUP did not return unimplemented")

  def test_set_mode_PULLDOWN(self):
      res = self.pin.mode(m.MODE_PULLDOWN)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting GPIO mode to MODE_PULLDOWN did not return unimplemented")

  def test_set_mode_HIZ(self):
      res = self.pin.mode(m.MODE_HIZ)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting GPIO mode to MODE_HIZ did not return unimplemented")

if __name__ == '__main__':
  u.main()
