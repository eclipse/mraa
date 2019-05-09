/*
 * Author: Dan Yocom <dan.yocom@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa'); //require mraa
console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

let myDigitalPin = new mraa.Gpio(5); //setup digital read on pin 5
myDigitalPin.dir(mraa.DIR_OUT); //set the gpio direction to output
myDigitalPin.write(1); //set the digital pin to high (1)
