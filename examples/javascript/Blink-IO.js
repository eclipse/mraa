/*
 * Author: Dan Yocom <dan.yocom@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

"use strict";

const mraa = require('mraa'); //require mraa
console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

let myLed = new mraa.Gpio(13); //LED hooked up to digital pin 13 (or built in pin on Galileo Gen1 & Gen2)
myLed.dir(mraa.DIR_OUT); //set the gpio direction to output
let ledState = true; //Boolean to hold the state of Led

function periodicActivity() {
    myLed.write(ledState ? 1 : 0); //if ledState is true then write a '1' (high) otherwise write a '0' (low)
    ledState = !ledState; //invert the ledState
}

setInterval(periodicActivity, 1000); //call the periodicActivity function every second
