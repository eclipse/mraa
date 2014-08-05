#######
Example
#######
Here are some examples of how to use mraa, common convention is to import mraa
as mraa to keep it short but feel free to import it globally! As a general rule
the API is very similar to the C++ API so there are only basic examples to show
quick usage. The mraa module can be built with python3 or python2. All examples
may not run on python3 as this is not tested.

Hello GPIO
==========

Here is the simplest Gpio program in mraa.

.. literalinclude:: ../../../examples/python/hello_gpio.py
  :prepend: import mraa
  :start-after: import mraa

GPIO Interupt (isr)
===================

The GPIO module allows you to set an interupt on a GPIO. This interupt is
controlled by the mode that the 'edge' is in.

**Note:** Galileo Gen1 only supports EDGE_BOTH

.. literalinclude:: ../../../examples/python/hello_isr.py
  :prepend: import mraa
  :start-after: import mraa

I2c
===

The I2c module module has a number of different ways of interacting with the
i2c bus, including a number of overloaded read() calls and the writeReg()
helper function.

.. literalinclude:: ../../../examples/python/i2c_HMC5883.py
  :prepend: x = mraa.I2c(0)
  :start-after: x = mraa.I2c(0)

Pwm
===

The PWM module is rather simple, note that different hardware support PWM
generation is various different ways so results may vary.

.. literalinclude:: ../../../examples/python/cycle-pwm3.py
  :prepend: import mraa
  :start-after: import mraa

Aio
===

The ADC is typically provided on a dedicated or shared SPI bus, this is
abstracted by the Linux kernel as spidev and abstracted again by mraa. It is
fairly simple in use.

.. literalinclude:: ../../../examples/python/aio.py
  :prepend: import mraa
  :start-after: import mraa

