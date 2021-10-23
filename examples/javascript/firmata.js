/*
 * Author: Ron Evans (@deadprogram)
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa');
console.log('MRAA Version: ' + mraa.getVersion());

// open connection to firmata
mraa.addSubplatform(mraa.GENERIC_FIRMATA, "/dev/ttyACM0");
