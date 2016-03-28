#!/usr/bin/env node

var m = require('mraa')

function h() {
  console.log("HELLO!!!!")
}

x = new m.Gpio(14)
x.isr(m.EDGE_BOTH, h)

setInterval(function() {
    // It's important to refer to our GPIO context here,
    // otherwise it will be garbage-collected
    console.log("Waiting for an interrupt at GPIO pin " + x.getPin() + "...")
}, 10000)
