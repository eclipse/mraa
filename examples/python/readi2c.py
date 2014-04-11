#!/usr/bin/env python2

import pymaa

x = pymaa.I2CSlave(27,28)
x.address(0x62)
y= "   "
ret = x.read(y, 2)

print(y)
