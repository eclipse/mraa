#######
Example
#######
Here are some examples of how to use mraa, common convention is to import mraa as mraa to keep it short but feel free to import it globally!

Hello GPIO
==========

Here is the simplest Gpio program in mraa.

.. literalinclude:: ../../../examples/python/hello_gpio.py
  :prepend: import mraa
  :start-after: import mraa

GPIO Interupt (isr)
===================

The GPIO module allows you to set an interupt on a GPIO. This interupt is controlled by the mode that the 'edge' is in.

**Note:** Galileo Gen1 only supports EDGE_BOTH

.. literalinclude:: ../../../examples/python/hello_isr.py
  :prepend: import mraa
  :start-after: import mraa

