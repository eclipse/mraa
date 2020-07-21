/*
 * Author: Dan Yocom <dan.yocom@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa'); //require mraa
console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

let analogPin0 = new mraa.Aio(0); //setup access analog inpuput pin 0
let analogValue = analogPin0.read(); //read the value of the analog pin
let analogValueFloat = analogPin0.readFloat(); //read the pin value as a float
console.log(analogValue); //write the value of the analog pin to the console
console.log(analogValueFloat.toFixed(5)); //write the value in the float format
