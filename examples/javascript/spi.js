#!/usr/bin/env node

/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa'); //require mraa

let spiDevice = new mraa.Spi(0);
let buf = new Buffer(4);
buf[0] = 0xf4;
buf[1] = 0x2e;
buf[2] = 0x3e;
buf[3] = 0x4e;
let buf2 = spiDevice.write(buf);
console.log("Sent: " + buf.toString('hex') + ". Received: " + buf2.toString('hex'));
