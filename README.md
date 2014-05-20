MAA - Low Level Skeleton Library for Communication on Intel platforms
==============

Library in C/C++ to interface with Galileo & other Intel platforms over:

- I2C
- SPI
- GPIO
- PWM
- AIO

In a structured and sane API with port nanmes/numbering that match boards &
with bindings to javascript & python.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

### ENV RECOMENDATIONS

All of these are 'optional', however they are recommended. Only a C compiler,
cmake and default system libraries are technically required to compile.

1. node.js 0.10.26
2. python 3.3.x or 2.7.x
3. swig-v8 3.0.1 (swig 2.x will work but you will not be able to build node.js
module so comment out add_directories(javacsript) in src/CmakeLists.txt)
4. doxygen (needed to generate even python doc)
5. sphinx
6. pygments
7. pydoc

To install swig-v8 the swig-v8-git package from AUR is helpful.

## COMPILING

NOTE: The **only** supported cmake build configuration is to have the build/ dir
inside of the repo/tarball.

*if swig-v8 is not in your default path you can try run cmake with
"-DCMAKE_PREFIX_PATH="/path/to/swig-v8"*

mkdir build/
cmake ..
make

## DEVELOPMENT

Please fork the code on github and then send pull requests. Please avoid merges
in your forks. I will also accept patches sent in git style with signoffs to
brendan.le.foll@intel.com

## USING

see examples/

*for node.js make sure that maajs.node is in the current dir and set export
NODE_PATH=.*

Simple python html documentation can be generated with **make pydoc**, this
does not require the full doxygen/sphinx setup.

## API Changelog

When the API breaks in a small way there will be a changelog listed here

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
