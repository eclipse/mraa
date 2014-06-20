MAA - Low Level Skeleton Library for Communication on Intel platforms
==============

Library in C/C++ to interface with Galileo & other Intel platforms, in a
structured and sane API with port nanmes/numbering that match boards & with
bindings to javascript & python.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

## API

These interfaces allow you to interact with all libmaa functionality. The C++
classes directly wrap the C API and provide a near 1:1 mapping of
functionality.

<center>
| C API Modules          | C++ API Classes             |
|:----------------------:|:---------------------------:|
| @ref gpio.h "gpio"     | @ref maa::Gpio "Gpio class" |
| @ref i2c.h "i2c"       | @ref maa::I2c "I2c class"   |
| @ref aio.h "aio"       | @ref maa::Aio "Aio class"   |
| @ref pwm.h "pwm"       | @ref maa::Pwm "Pwm class"   |
| @ref spi.h "spi"       | @ref maa::Spi "Spi class"   |
| @ref common.h "common" | @ref common.h "common"      |
</center>

### Hello Maa
@snippet hellomaa.c Interesting

## Supported platforms

Specific platform information for supported platforms is documented here:

- @ref galileorevd

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

**0.3.1**
  * Initial Intel Galileo Gen 2 support
  * maa_gpio_isr parameters added.
  * Detection of different supported platforms added.

**0.3.0**
  * maa_i2c_read now returns length of read

**0.2.9**
  * Add global maa.h & maa.hpp headers
  * usage of "gpio.h" is not legal you need to use "maa/gpio.h" unless adding
    -L/usr/include/maa

**0.2.8**
  * Added maa_set_priority call
  * Added mmap gpio call maa_gpio_use_mmaped

**0.2.7**
  * C++ API now uses basic types and not unistd types as C
  * Clearer and consistent use of unistd tpyes in C api

**0.2.6**
  * C++ examples added, using c++ headers/api.
  * moved to open instead of fopen in all modules
  * rename maa_check functions and made them internal to maa only.
  * removed "export" functions from api
  * Intel Galileo Gen 1 (rev d) fixes, incorrect definition of some items
  * SPI, implementation completed.
  * I2C internal function, completed.
  * PWM fix bug in period set method.
  * Swig upstream can be used for building.
  * No longer builds docs on default cmake, needs flag
  * GPIO/PWM ownership guard prevents closing on existing pins, still can be forced.

**0.2.5**
  * C++/Python/Node Enums/const names now do not contain MAA_GPIO
  * Enum type declaration for C++ changed
  * Python/Node get_version() -> GetVersion()
  * i2c read calls don't use const char* and i2c write calls do

**0.2.4**
  * All maa_ contexts now are pointers to the actual struct which is not
    delcared in the header. The only end user change is that instead of
    returning a type maa_gpio_context pointer maa_gpio_init() now returns a
    maa_gpio_context (which is actually a pointer to struct _gpio internally).
  * C++ API is available, use the .hpp headers and add stdc++  to the linker
    flags.
  * Initial SPI implementation is provided

**0.2.3**
  * maa_aio_read_u16() -> maa_aio_read()
  * maa_aio_read() now returns a uint16_t instead of an unsigned int

**0.2.2**
  * First version with API considered 'stable'
