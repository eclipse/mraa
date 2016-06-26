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

class GpioChecksDir(u.TestCase):
  def setUp(self):
    self.pin = m.Gpio(MRAA_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_set_dir_output(self):
    direction = m.DIR_OUT
    res = self.pin.dir(direction)
    self.assertEqual(res, m.SUCCESS, "Setting GPIO to output failed")
    self.assertEqual(self.pin.readDir(), direction, "GPIO has incorrect direction after dir(DIR_OUT)")

  def test_set_dir_output_HIGH(self):
    res = self.pin.dir(m.DIR_OUT_HIGH)
    self.assertEqual(res, m.SUCCESS, "Setting GPIO to output HIGH failed")
    self.assertEqual(self.pin.readDir(), m.DIR_OUT, "GPIO has incorrect direction after dir(DIR_OUT_HIGH)")
    self.assertEqual(self.pin.read(), 1, "GPIO has incorrect state after dir(DIR_OUT_HIGH)")

  def test_set_dir_output_LOW(self):
    res = self.pin.dir(m.DIR_OUT_LOW)
    self.assertEqual(res, m.SUCCESS, "Setting GPIO to output LOW failed")
    self.assertEqual(self.pin.readDir(), m.DIR_OUT, "GPIO has incorrect direction after dir(DIR_OUT_LOW)")
    self.assertEqual(self.pin.read(), 0, "GPIO has incorrect state after dir(DIR_OUT_LOW)")

  def test_set_dir_input(self):
    direction = m.DIR_IN
    res = self.pin.dir(direction)
    self.assertEqual(res, m.SUCCESS, "Setting GPIO to input failed")
    self.assertEqual(self.pin.readDir(), direction, "GPIO has incorrect direction after dir(DIR_IN)")

  def test_set_dir_invalid(self):
    direction = 99
    res = self.pin.dir(direction)
    self.assertNotEqual(res, m.SUCCESS, "Setting direction to " + str(direction) + " should have failed")

if __name__ == '__main__':
  u.main()
