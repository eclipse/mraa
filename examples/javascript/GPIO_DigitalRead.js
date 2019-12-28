/*
 * Author: Dan Yocom <dan.yocom@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa'); //require mraa
console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

let myDigitalPin = new mraa.Gpio(6); //setup digital read on pin 6
myDigitalPin.dir(mraa.DIR_IN); //set the gpio direction to input

function periodicActivity() {
    let myDigitalValue = myDigitalPin.read(); //read the digital value of the pin
    console.log('Gpio value is ' + myDigitalValue); //write the read value out to the console
}

setInterval(periodicActivity, 1000); //call the indicated function every 1 second (1000 milliseconds)
