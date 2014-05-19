#!/usr/bin/env python

import pymaa as maa

def test():
  print("wooo")

x = maa.Gpio(6)
x.dir(maa.DIR_IN)
x.isr(maa.EDGE_BOTH, test)
