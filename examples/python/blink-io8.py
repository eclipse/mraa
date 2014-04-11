#!/usr/bin/env python3

import pymaa as maa
import time

x = maa.gpio_t()
maa.gpio_init(x, 26)
maa.gpio_dir(x, "out")

while True:
    maa.gpio_write(x,1)
    time.sleep(0.2)
    maa.gpio_write(x,0)
    time.sleep(0.2)
