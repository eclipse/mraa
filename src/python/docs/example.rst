#######
Example
#######
Here are some examples of how to use pymaa, common convention is to import pymaa as maa to keep it short but feel free to import it globally!

Hello GPIO
==========

Here is the simplest Gpio program in pymaa.

.. literalinclude:: ../../../examples/python/hello_gpio.py
  :prepend: import pymaa
  :start-after: import pymaa

GPIO Interupt (isr)
===================

The GPIO module allows you to set an interupt on a GPIO. This interupt is controlled by the mode that the 'edge' is in.

**Note:** Galileo Gen1 only supports EDGE_BOTH

.. literalinclude:: ../../../examples/python/hello_isr.py
  :prepend: import pymaa as maa
  :start-after: import pymaa as maa

