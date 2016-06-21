#######
Example
#######
Here are some examples of how to use mraa, common convention is to
require('mraa'). As a general rule the API is very similar to the C++ API so
there are only basic examples to show quick usage. The mraa module can be
built with lua 5.1+, luajit 5.1+, Tarantool.

Hello GPIO
==========

Here is the simplest Gpio program in mraa.

.. literalinclude:: ../../../examples/lua/hello_gpio.lua

  :prepend: require('mraa')
  :start-after: require('mraa')

Aio
===

The ADC is typically provided on a dedicated or shared SPI bus, this is
abstracted by the Linux kernel as spidev and abstracted again by mraa. It is
fairly simple in use.

.. literalinclude:: ../../../examples/lua/aio.lua

  :prepend: require('mraa')
  :start-after: require('mraa')

Pwm
===

The PWM module is rather simple, note that different hardware support PWM
generation is various different ways so results may vary.

.. literalinclude:: ../../../examples/lua/cycle-pwm3.lua

  :prepend: require('mraa')
  :start-after: require('mraa')

