#!/usr/bin/env python3

import pymaa as maa
import time

x = maa.PWM(0,3)
x.enable(1);
x.period_us(20)
value= 0.0

while True:
    x.write(value)
    time.sleep(0.2)
    value = value +0.01
    if value >= 1:
        value = 0.0
