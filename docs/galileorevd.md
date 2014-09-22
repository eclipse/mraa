Galileo Gen 1 - Rev D                            {#galileorevd}
=====================

Galileo is a microcontroller board based on the Intel® Quark SoC X1000
Application Processor, a 32-bit Intel Pentium-class system on a chip.

The rev D board has the following limitations in libmraa:

- gpio register access via /dev/uio is limited to pin2 and 3
- gpio interupts will only work on GPIO_EDGE_BOTH
- adc kernel module will return 12bit number. MRAA defaults shift this to 10bits
