Intel Edison                            {#edison}
=============

Edison is a dual core Silvermont Atom clocked at 500MHz. The Edison
also features 4GB of storage, 1GB ram and onboard wifi and bluetooth.

Currently the Arduino breakout board is supported by libmraa.

The Edison used with the Arduino board has the following limitations
in libmraa:

- PWM avaible on default swizzler postions. (3,5,6,9)
- SPI exposed is also used for the ADC. Try not to use your own CS.
- ADC kernel module will return 12bit number but the ADC itself only has an
  accuracy of 10bits. This ADC is only included on the arduino board.
