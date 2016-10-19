Galileo Gen 1 (Rev D)                            {#galileogen1}
=====================

Galileo is a microcontroller board based on the Intel(R) Quark(TM) SoC X1000
Application Processor, a 32-bit Intel Pentium-class system on a chip. The gen1
board is smaller than the gen2 board and can be easily identified by it's lack
of full size USB port and it has a 3.5mm jack serial connector.

The gen1 board has the following limitations in libmraa:

- gpio 13 will not switch the LED as it's a different Gpio, use raw gpio '3' to do this
- gpio register access via /dev/uio is limited to pin2 and 3
- gpio interrupts will only work on GPIO_EDGE_BOTH
- adc kernel module will return 12bit number. MRAA defaults shift this to 10bits
- AIO pins are treated as 0-5 in mraa_aio_init() but as 14-19 for everything
  else. Therefore use mraa_gpio_init(14) to use A0 as a Gpio
