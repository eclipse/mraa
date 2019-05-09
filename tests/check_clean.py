#!/usr/bin/python
# Copyright (c) 2016 Intel Corporation.
# SPDX-License-Identifier: MIT

import unittest as u
import re, fnmatch, os

rootDir = 'src/java'
swigtypeStr = 'SWIGTYPE'

class Clean(u.TestCase):

    def test_existing_swigtype(self):
        unclean = []

        for fileName in os.listdir(rootDir):
            if swigtypeStr in fileName:
                unclean.append(fileName)

        self.assertEqual( len(unclean), 0,
                "\nmraa contains unclean Java bindings:\n" + \
                "\n".join(unclean) + "\n\n")

if __name__ == '__main__':
    u.main()
