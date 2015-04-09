Beaglebone Black    {#beaglebone}
================

The Beaglebone Black is a very maker friendly Board with a huge amount of
available I/O Pins. It consists of an Cortex-A8 single core CPU plus two
additional microcontroller cores called 'pru' that can be used for realtime
tasks.

The official Beaglebone Black Image runs Debian on a 3.8.13 Kernel. But there
are also mainline kernels available, either from Robert C. Nelson or also as
part of the upcoming Fedora 22 release.

The kernel releases from Robert C. Nelson have usually more complete support as
not all code is yet commited to mainline kernel, your mileage may vary!

In Kernel 3.8.13 there is a Capemanager included, a mechanism to load
configuration data for devices and extension boards from userland.

This mechanism does not (yet) exist in Mainline kernels, so for mainline
kernels you need to either rely on the pre-delivered devicetree's or you will
need to build your own devicetree to support hardware not available by default.

Revision Support
----------------
Beaglebone Black Rev. B
Beaglebone Black Rev. C

Interface notes
---------------

**SPI** works fine with 3.8.13 kernels, on Mainline Kernel SPI does currently
not work. mraa will activate spi on 3.8.13 if it finds out that spi is not yet
configured

**I2C** works both on 3.8.13 and mainline. i2c is activated if missing for
3.8.13 kernels

Mainline Kernel requires the use of Device-Trees, mraa tries it's best to guess
which gpio/serial/i2c/spi is connected where but there is currently no support
to manipulate the Device-Tree settings from within mraa.  If a device does not
work as expected then please check syslog, mraa usually complains with a
meaningful message when it is unable to initialize the device.

It will also tell you which overlay for SPI/COM/I2C/PWM it tries to load, on
some older Debian distributions (or heaven forbid, on Angström) you may need to
install thoses overlays to /lib/firmware

Capes and further documentation
-------------------------------

Correctly configuring i2c/spi/serial can get a little challenging as some pins
have double functionality or are not available at all because hdmi is enabled.
When something does not work as expected make sure to first check the syslog,
then check the Beaglebone documentation. Some pointers for good descriptions
are:

http://elinux.org/BeagleBone_Black_Enable_SPIDEV
http://elinux.org/Interfacing_with_I2C_Devices

When working with mainline kernels take every hit you have on google with a
grain of salt, a lot of documentation is based on 3.8 and older kernels. Using
mainline kernels can be very rewarding, but at least at time of writing also
can have some nasty
pitfalls.

Pin Mapping
-----------

mraa will take into account if you have hdmi cape or mmc enabled and will show
you the gpio's available for your given configuration.

To see the pin mapping use the command:

$ sudo mraa-gpio list
