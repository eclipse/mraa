#!/usr/bin/env node

var m = require('mraa')

function h() {
  console.log("HELLO!!!!")
}

x = new m.Gpio(14)
x.isr(m.EDGE_BOTH, h)
