#!/usr/bin/env python

import pymaa as maa

def test():
  print("wooo")

x = maa.Gpio(6)
x.dir(maa.MAA_GPIO_IN)
x.isr(maa.MAA_GPIO_EDGE_BOTH, test)
