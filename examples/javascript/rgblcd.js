/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa'); //require mraa

let i2cDevice = new mraa.I2c(0);
i2cDevice.address(0x62);
i2cDevice.writeReg(0, 0);
i2cDevice.writeReg(1, 0);

i2cDevice.writeReg(0x08, 0xAA);
i2cDevice.writeReg(0x04, 255);
i2cDevice.writeReg(0x02, 255);
