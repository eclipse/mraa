#!/usr/bin/env node

"use strict";

const mraa = require('mraa');

function hello() {
    console.log("HELLO!!!!");
}

let pin = new mraa.Gpio(14);
pin.isr(mraa.EDGE_BOTH, hello);

setInterval(function() {
    // It's important to refer to our GPIO context here,
    // otherwise it will be garbage-collected
    console.log("Waiting for an interrupt at GPIO pin " + pin.getPin() + "...");
}, 10000);
