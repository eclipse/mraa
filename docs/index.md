libmraa - Low Level Skeleton Library for Communication on GNU/Linux platforms
==============

Libmraa is a C/C++ library with bindings to Python, Javascript and Java to
interface with the I/O on Galileo, Edison & other platforms, with a structured
and sane API where port names/numbering matches the board that you are on. Use
of libmraa does not tie you to specific hardware with board detection done at
runtime you can create portable code that will work across the supported
platforms.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

## API

These interfaces allow you to interact with all libmraa functionality. The C++
classes directly wrap the C API and provide a near 1:1 mapping of
functionality.

<center>
| C API Modules          | C++ API Classes              |
|:----------------------:|:----------------------------:|
| @ref gpio.h "gpio"     | @ref mraa::Gpio "Gpio class" |
| @ref i2c.h "i2c"       | @ref mraa::I2c "I2c class"   |
| @ref aio.h "aio"       | @ref mraa::Aio "Aio class"   |
| @ref pwm.h "pwm"       | @ref mraa::Pwm "Pwm class"   |
| @ref spi.h "spi"       | @ref mraa::Spi "Spi class"   |
| @ref uart.h "uart"     | @ref mraa::Uart "Uart class" |
| @ref common.h "common" | @ref mraa "common"           |
</center>

### Hello Mraa
@snippet hellomraa.c Interesting

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
- @ref nuc5
- @ref ft4222

## DEBUGGING

Sometimes it just doesn't want to work, let us try and help you, you can file
issues in github or join us in #mraa on freenode IRC, hang around for a little
while because we're not necessarily on 24/7, but we'll get back to you! Have a
glance at our @debugging page too

## COMPILING

More information on compiling is @ref building page.

## CONTRIBUTING

Please see the @ref contributing page, the @ref internals page may also be of
use.

## API Changelog

This changelog is meant as a quick & rough guide to what has changed between
versions. The API is now fairly stable but when new calls/features are added
they are listed here. Anything pre 0.2.x is ignored.

**0.8.0**
  * Better java support
  * Support for FT4222 subplatforms
  * New types.hpp improves C++ & SWIG APIs
  * Added support for minnowboard max compatible boards

**0.7.5**
  * 5th Generation NUC i2c support
  * NPM 0.10.x ia32 fix

**0.7.4**
  * Minnowboard i2c fix
  * Add NPM arm support
  * Uart initialisation improved
  * Better i2c bus detection on DE3815 & mmax

**0.7.3**
  * DE3815 i2c fix when using different kernels
  * Fixed various memory leaks in SWIG wrappers
  * gpio enums all prefixed with mraa_
  * Fix SPI CS pin caps causing mux_total to be > 0
  * Improved error checking/handling

**0.7.2**
  * Fix node.js npm builds with binding.gyp that didn't use --target-arch

**0.7.1**
  * Uart now uses binary arrays and has optional 'Str' functions in C++/SWIG
  * Various Uart module bugfixes
  * Node.js 0.12.4 support
  * Node.js documentation support

**0.7.0**
  * Uart module now contains read/write interactions and configuration
  * Python API contains more buffer checks
  * Java support
  * RPM cpack support

**0.6.2**
  * Node.js 0.12 support
  * NPM support
  * Formatting done with clang-format
  * Various examples and documentation updates
  * Supported added for Beaglebone Black + Banana Pi

**0.6.1**
  * 16bit spi iunctions added
  * Node.js ISR now supported

**0.6.0**
  * add device path queries for uart
  * add platform configuration querying
  * gpio sample added
  * improve i2c/spi write/read API for python & nodejs
  * performance improvements on edison & galileo

**0.5.4**
  * pwm read_* fix introduced in 0.5.3
  * improved npmpkg support

**0.5.3**
  * OE toolchain support added to CMake
  * Various UART fixes
  * SPI add CS exposure
  * Remove functions from mraa.c into modules
  * Rework of support for mmap
  * Intel Edison MMAP support added. Read and Write
  * I2C clean up, add cleaner functions
  * MinnowBoard Max support added
  * PWM period is written before duty
  * Node GYP build supported added
  * Add Get Platform Name function

**0.5.2**
  * pwm improvement & bugfix
  * spi mraa_spi_bit_per_word fix
  * new spi transfer function
  * i2c object api uses uint8_t
  * i2c readReg() calls added
  * edison i2c bus now defaults to a sensible value
  * edison uart enable support
  * edison hardware CS exposed as IO10
  * DE3815tykhe NUC support

**0.5.1**
  * Intel Edison - Mini breakout board support
  * Change to use syslog throughout instead of printing to stderr.
  * Fix misc issues brought up throuh coverity scans
  * Clear up Analog call documentation

**0.5.0**
  * Intel Edison - Arduino board support.
  * Boost Allowable i2c busses to 12
  * Additional platform hooks added
  * ADC is now 10bits by default on all platforms but can be changed

**0.4.5**
  * if only one spidev we default to it reguardless of number
  * mraa_pwm_config_ms & mraa_pwm_config_percent functions added
  * Uart C++ class added, adds python & node support
  * galileo gen2 gpio modes supported

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
