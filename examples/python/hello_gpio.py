#!/usr/bin/env python3

import pymaa

x = pymaa.gpio_t()
print(x.pin)
gpio_init(x, 20)
print(x.pin)
