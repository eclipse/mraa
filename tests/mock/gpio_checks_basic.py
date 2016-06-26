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

class GpioChecksBasic(u.TestCase):
  def setUp(self):
    self.pin = m.Gpio(MRAA_TEST_PIN)

  def tearDown(self):
    del self.pin

  def test_gpio_state_after_init(self):
      # After GPIO init it should be in INPUT and LOW state
      self.assertEqual(self.pin.read(), 0, "GPIO is in a wrong state after init")

  def test_gpio_dir_after_init(self):
      # After GPIO init it should be in INPUT and LOW state
      self.assertEqual(self.pin.readDir(), m.DIR_IN, "GPIO has wrong direction after init")

  def test_get_pin_num(self):
    self.assertEqual(self.pin.getPin(), MRAA_TEST_PIN, "Returned GPIO pin number is incorrect")

if __name__ == '__main__':
  u.main()
