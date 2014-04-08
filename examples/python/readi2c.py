#!/usr/bin/env python2

import pymaa

x = pymaa.I2C(27,28)
y= "   "
ret = x.read(0x62, y, 2)

print(y)
