#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko <alext.mkrs@gmail.com>
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

MRAA_TEST_PIN = 0

class GpioChecksEdge(u.TestCase):
  def setUp(self):
    self.pin = m.Gpio(MRAA_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_set_edge_mode_NONE(self):
      res = self.pin.edge(m.EDGE_NONE)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting edge mode to EDGE_NONE did not return unimplemented")

  def test_set_edge_mode_BOTH(self):
      res = self.pin.edge(m.EDGE_BOTH)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting edge mode to EDGE_BOTH did not return unimplemented")

  def test_set_edge_mode_RISING(self):
      res = self.pin.edge(m.EDGE_RISING)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting edge mode to EDGE_RISING did not return unimplemented")

  def test_set_edge_mode_FALLING(self):
      res = self.pin.edge(m.EDGE_FALLING)
      self.assertEqual(res, m.ERROR_FEATURE_NOT_IMPLEMENTED, "Setting edge mode to EDGE_FALLING did not return unimplemented")

if __name__ == '__main__':
  u.main()
