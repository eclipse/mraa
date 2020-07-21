#!/usr/bin/env python

# Author: Costin Constantin <costin.c.constantin@intel.com>
# Copyright (c) 2015 Intel Corporation.
#
# SPDX-License-Identifier: MIT

from __future__ import print_function

import mraa as m
import unittest as u

class GeneralChecks(u.TestCase):
  def test_mraa_version(self):
    self.version = m.getVersion()
    print("Version is: " + self.version)
    self.assertIsNotNone(self.version)

if __name__ == "__main__":
  u.main()
