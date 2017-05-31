#!/usr/bin/env python

# Author: Alex Tereschenko <alext.mkrs@gmail.com>
# Copyright (c) 2016 Alex Tereschenko <alext.mkrs@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
