#!/usr/bin/python
# Copyright (c) 2016 Intel Corporation.
# SPDX-License-Identifier: MIT

import unittest as u
import re, fnmatch, os, sys

sampleMappingFile = '../examples/samples.mapping.txt'
cSamplesDir = '../examples/'
javaSamplesDir = '../examples/java/'
cppSamplesDir = '../examples/c++/'

class SampleNames(u.TestCase):

    def test_existing_samples(self):
        missing_c_files = []
        missing_cpp_files = []
        missing_java_files = []

        with open (sampleMappingFile, "r") as f:
            for line in f:
                sampleNames = line.split();

                cSampleName = sampleNames[0]
                javaSampleName = sampleNames[1]

                #check for C files
                if cSampleName.endswith('.c'):
                    ok = False
                    for file in os.listdir(cSamplesDir):
                        if file == cSampleName:
                            ok = True
                            break
                    if not ok:
                        missing_c_files.append(cSampleName)

                #check for Cpp files
                if cSampleName.endswith('.cpp'):
                    ok = False
                    for file in os.listdir(cppSamplesDir):
                        if file == cSampleName:
                            ok = True
                            break
                    if not ok:
                        missing_cpp_files.append(cSampleName)

                #check for java files
                javaSampleName = javaSampleName.lstrip("java/")
                if javaSampleName.endswith('.java'):
                    ok = False
                    for file in os.listdir(javaSamplesDir):
                        if file == javaSampleName:
                            ok = True
                            break
                    if not ok:
                        missing_java_files.append(javaSampleName)

        self.assertEqual( len(missing_java_files) + len(missing_c_files) + len(missing_cpp_files), 0,
                "\nThe following files are missing from samples:\n" + \
                "\n".join(missing_c_files) + "\n" + \
                "\n".join(missing_cpp_files) + "\n" + \
                "\n".join(missing_java_files))

if __name__ == '__main__':
          u.main()

