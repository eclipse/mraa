#!/usr/bin/env bash

# Author: Nicolas Oliver <dario.n.oliver@intel.com>
# Copyright (c) 2017 Intel Corporation.
# SPDX-License-Identifier: MIT

set -x
set -e

# Install doxygen2jsdoc dependencies
cd doxygen2jsdoc && npm install && cd ..

# Install doxyport dependencies
cd doxyport && make setup && cd ..

# Make Documentation
make -j8 -Cbuild

# Make Java Documentation
cd build/src/java && echo ../../../src/mraa.i > mraa.i.list && \
../../../doxyport/doxyport mraa.i.list \
    --cmake ../../compile_commands.json \
    --source ../../../api,../../../api/mraa \
    --destination $(pwd)/ \
    --convert-protected-to-private \
    --output mraa-java-files.txt \
    --mapping ../../../examples/samples.mapping.txt && \
doxygen Doxyfile && cd ../../../

# Copy output to build/html/ directory
cp -r build/src/python/python2/docs/html build/html/python && \
cp -r build/src/java/html build/html/java && \
cp build/jsdoc/ternjs/mraa/doc.js build/html/node/mraa_tern.js
