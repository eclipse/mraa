var m = require("maajs")

console.log("maa version: " + m.get_version());

var r = new m.I2C(20, 21);
