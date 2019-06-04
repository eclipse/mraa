libmraa - Low Level Skeleton Library for Communication on GNU/Linux platforms
==============

Libmraa is a C/C++ library with bindings to Java, Python and JavaScript to
interface with the I/O on Galileo, Edison & other platforms, with a structured
and sane API where port names/numbering matches the board that you are on. Use
of libmraa does not tie you to specific hardware with board detection done at
runtime you can create portable code that will work across the supported
platforms.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

## API

These interfaces allow you to interact with all libmraa functionality. The
Java classes directly wrap the C/C++ API and provide a near 1:1 mapping of
functionality.

<center>
| C API Modules          | Java API Classes             |
|:----------------------:|:----------------------------:|
| @ref gpio.h "gpio"     | @ref mraa::Gpio "Gpio class" |
| @ref led.h "led"       | @ref mraa::Led "Led class"   |
| @ref i2c.h "i2c"       | @ref mraa::I2c "I2c class"   |
| @ref aio.h "aio"       | @ref mraa::Aio "Aio class"   |
| @ref pwm.h "pwm"       | @ref mraa::Pwm "Pwm class"   |
| @ref spi.h "spi"       | @ref mraa::Spi "Spi class"   |
| @ref uart.h "uart"     | @ref mraa::Uart "Uart class" |
| @ref common.h "common" | @ref mraa::mraa "mraa class" |
</center>

### Hello Mraa
@snippet HelloEdison.java Interesting

## Supported platforms

Specific platform information for supported platforms is documented here:

- @ref galileorevd
- @ref galileorevh
- @ref edison
- @ref de3815
- @ref minnowmax
- @ref rasppi
- @ref bananapi
- @ref beaglebone
- @ref phyboard-wega
- @ref nuc5
- @ref up
- @ref joule
- @ref ft4222
- @ref iei-tank
- @ref up-xtreme

## DEBUGGING

Sometimes it just doesn't want to work, let us try and help you, you can file
issues in github or join us in #mraa on freenode IRC, hang around for a little
while because we're not necessarily on 24/7, but we'll get back to you! Have a
glance at our @ref debugging page too

## COMPILING

More information on compiling is @ref building page.

## CONTRIBUTING

Please see the @ref contributing page, the @ref internals page may also be of
use.

## API Changelog

Version @ref changelog here.
