MRAA - Low Level Skeleton Library for Communication on Intel platforms
==============

Library in C/C++ to interface with Galileo & other Intel platforms, in a
structured and sane API with port names/numbering that match boards & with
bindings to javascript & python.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

## API

These interfaces allow you to interact with all libmraa functionality. The C++
classes directly wrap the C API and provide a near 1:1 mapping of
functionality.

<center>
| C API Modules          | C++ API Classes             |
|:----------------------:|:---------------------------:|
| @ref gpio.h "gpio"     | @ref mraa::Gpio "Gpio class" |
| @ref i2c.h "i2c"       | @ref mraa::I2c "I2c class"   |
| @ref aio.h "aio"       | @ref mraa::Aio "Aio class"   |
| @ref pwm.h "pwm"       | @ref mraa::Pwm "Pwm class"   |
| @ref spi.h "spi"       | @ref mraa::Spi "Spi class"   |
| @ref common.h "common" | @ref common.hpp "common"     |
</center>

### Hello Mraa
@snippet hellomraa.c Interesting

## Supported platforms

Specific platform information for supported platforms is documented here:

- @ref galileorevd
- @ref galileorevh

### ENV RECOMENDATIONS

All of these are 'optional', however they are recommended. Only a C compiler,
cmake, libm and pthreads are technically required to compile.

- Swig 3.0.1+ built with node.js & python support (0.10.x)
- doxygen
- sphinx (requires doxygen)
- pygments

## COMPILING

More information on compiling is @ref building page

## CONTRIBUTING

Please see the @ref contributing page

## API Changelog

This changelog is meant as a quick & rough guide to what has changed between
versions. The API is now fairly stable but when new calls/features are added
they are listed here. Anything pre 0.2.x is ignored.

**0.4.4**
  * prefix SPI mode with MRAA_SPI_
  * added C++ adc bitness calls

**0.4.3**
  * SPI Mode function now functional, API Change in SPI
  * I2C read in swig worked on.
  * Galileo Gen 2: PWM setting period sets all channel's period
  * Galileo Gen 2: I2C setup now specific to Gen 2.
  * General commits around freeing memory at the right times.

**0.4.2**
  * Barebone UART module added.
  * Hook branch merged.
  * I2C init hooks added.
  * Intel Galileo Gen 2, I2C gpio pins now go hiz input when I2C initialised.

**0.4.1**
  * Rename python & nodejs modules to mraa
  * common.hpp introduced for C++
  * python & nodejs modules can now take binary strings for Spi & I2c
  * improved Aio module and clear bitness
  * Improved Galileo Gen 2 support

**0.4.0**
  * Rename to mraa
  * get_platform_type function added.

**0.3.1**
  * Initial Intel Galileo Gen 2 support
  * mraa_gpio_isr parameters added.
  * Detection of different supported platforms added.

**0.3.0**
  * mraa_i2c_read now returns length of read

**0.2.9**
  * Add global mraa.h & mraa.hpp headers
  * usage of "gpio.h" is not legal you need to use "mraa/gpio.h" unless adding
    -L/usr/include/mraa

**0.2.8**
  * Added mraa_set_priority call
  * Added mmap gpio call mraa_gpio_use_mmaped

**0.2.7**
  * C++ API now uses basic types and not unistd types as C
  * Clearer and consistent use of unistd tpyes in C api

**0.2.6**
  * C++ examples added, using c++ headers/api.
  * moved to open instead of fopen in all modules
  * rename mraa_check functions and made them internal to mraa only.
  * removed "export" functions from api
  * Intel Galileo Gen 1 (rev d) fixes, incorrect definition of some items
  * SPI, implementation completed.
  * I2C internal function, completed.
  * PWM fix bug in period set method.
  * Swig upstream can be used for building.
  * No longer builds docs on default cmake, needs flag
  * GPIO/PWM ownership guard prevents closing on existing pins, still can be forced.

**0.2.5**
  * C++/Python/Node Enums/const names now do not contain MRAA_GPIO
  * Enum type declaration for C++ changed
  * Python/Node get_version() -> GetVersion()
  * i2c read calls don't use const char* and i2c write calls do

**0.2.4**
  * All mraa_ contexts now are pointers to the actual struct which is not
    delcared in the header. The only end user change is that instead of
    returning a type mraa_gpio_context pointer mraa_gpio_init() now returns a
    mraa_gpio_context (which is actually a pointer to struct _gpio internally).
  * C++ API is available, use the .hpp headers and add stdc++  to the linker
    flags.
  * Initial SPI implementation is provided

**0.2.3**
  * mraa_aio_read_u16() -> mraa_aio_read()
  * mraa_aio_read() now returns a uint16_t instead of an unsigned int

**0.2.2**
  * First version with API considered 'stable'
