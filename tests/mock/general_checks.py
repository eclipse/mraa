#!/usr/bin/env python

# Author: Costin Constantin <costin.c.constantin@intel.com>
# Copyright (c) 2015 Intel Corporation.
#
# Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
#
# SPDX-License-Identifier: MIT

import mraa as m
import unittest as u

class GeneralChecks(u.TestCase):
  def test_mraa_version(self):
    version = m.getVersion()
    print("Version is: " + version)
    self.assertIsNotNone(version)
    self.assertNotEqual(version, "", "MRAA version is an empty string")

if __name__ == "__main__":
  u.main()
