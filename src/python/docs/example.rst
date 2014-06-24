#######
Example
#######
Here are some examples of how to use pymraa, common convention is to import pymraa as mraa to keep it short but feel free to import it globally!

Hello GPIO
==========

Here is the simplest Gpio program in pymraa.

.. literalinclude:: ../../../examples/python/hello_gpio.py
  :prepend: import pymraa
  :start-after: import pymraa

GPIO Interupt (isr)
===================

The GPIO module allows you to set an interupt on a GPIO. This interupt is controlled by the mode that the 'edge' is in.

**Note:** Galileo Gen1 only supports EDGE_BOTH

.. literalinclude:: ../../../examples/python/hello_isr.py
  :prepend: import pymraa as mraa
  :start-after: import pymraa as mraa

