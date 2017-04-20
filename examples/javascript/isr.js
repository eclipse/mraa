#!/usr/bin/env node

var mraa = require('mraa');

function hello() {
    console.log("HELLO!!!!");
}

let pin = new m.Gpio(14);
pin.isr(m.EDGE_BOTH, hello);

setInterval(function() {
    // It's important to refer to our GPIO context here,
    // otherwise it will be garbage-collected
    console.log("Waiting for an interrupt at GPIO pin " + pin.getPin() + "...");
}, 10000);
