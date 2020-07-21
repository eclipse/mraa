Changelog                         {#changelog}
=========

This changelog is meant as a quick & rough guide to what has changed between
versions. The API is now fairly stable but when new calls/features are added
they are listed here. Anything pre 0.2.x is ignored.

**2.1.0**
  * New platforms UP Xtreme, IPi SMARC, ROCK PI 4, Raspberry Pi 4, Ultra96
  * Added support for chardev GPIO devices on Rock960
  * Introduced APIs to initialize GPIOs by name
  * Multiple fixes to address static tool scans and user submitted bug reports
  * Revised documentation to reflect project transfer to the Eclipse Foundation
  * Updated installation instructions for multiple distribution channels

**2.0.0**
  * New platforms IEI Tank, Ultra-96, Rock960, Hikey960, Dragonboard 820c
  * Added support for chardev GPIO devices with implementation on several boards
  * Added string based I/O initialization for simpler integration with other frameworks
  * Added GTest support for unit testing
  * Extended named LED device APIs allowing them to be listed during board definition
  * Removed packaging and CI for obsolete devkits
  * Multiple bug fixes and cmake improvements

**1.9.0**
  * Added support for RPi Zero W
  * Added support for MIPS based Omega2 and Linkit Smart 7688
  * New APIs for sysfs onboard LED control using the gpio-leds driver
  * Restructured and cleaned-up examples
  * Improved documentation generation and CI integration
  * Static analysis fixes, added SonarQube badge
  * Enhanced Node.js detection on some Linux distros (OpenSUSE)

**1.8.0**
  * Added Up2 support & grovePi subplatform support
  * Various improvements on 96board, rpi, beaglebone & Up boards
  * CMake 3.8+ improvements
  * Peripheral I/O improvements
  * Json platform improvements
  * Static analysis cleanup using SonarQube

**1.7.0**
  * Peripheral I/O support for Android Things
  * Deprecation of node v0.10.x and 0.12.x
  * Documentation improvements

**1.6.1**
  * mraa-deinit bug fix
  * rpi3 fix
  * ALTERA_SOCFPGA -> DE_NANO_SOC

**1.6.0**
  * de-10 nano board support
  * Improved Intel Joule support & renamed from GT_TUCHUCK to Joule
  * Improved RPI3 support
  * Fixes for UP, 96boards & phyboard-wega
  * Call mraa_deinit with a gcc destructor attribute

**1.5.1**
  * Small memleaks plugged
  * imraa useless arp statements removed
  * NPM 1.5.0 did not 100% match real 1.5.0 tag, this resolves this

**1.5.0**
  * imraa now uses argp, slight option changes, you need to use -a to flash a 101
  * Joule i2c enumeration much improved, uses pci id
  * Firmata add a spinlock to remove possible race condition
  * Mock Uart functionality

**1.4.0**
  * Add support for Siemens SIMATIC IOT2000
  * Cmake now enables much more error detection depending on compiler support
  * Mraa Maven packages now enabled
  * Galileo Gen2 AIO fix using pincmd
  * Adding an already added firmata platform now returns MRAA_SUCCESS
  * Cmake no longer tags builds without git as '-dirty' version.c for easier
    packaging support

**1.3.0**
  * Fix GT GPIO bugs
  * Mock SPI and travis now uses mock and tests it
  * RPI zero support

**1.2.3**
  * Revert 1.2.1 binding.gyp changed that had side effect of enabling JSONPLAT in mraa.c
  * improve travis.ci to load mraa.node as compiled from make npmpkg target

**1.2.2**
  * remove jsonplat from prehashed npm builds (see docs/npm.md for details)
  * internal spelling fix

**1.2.1**
  * Fix npm pkg build - no other fix

**1.2.0**
  * JSON platform support
  * mock I2c functionality
  * Intel Joule PWM fix
  * AIO firmata bug fix

**1.1.2**
  * Mock platform support
  * mraa-i2c treats i2c buses by default as linux
  * Intel Joule i2c fixes
  * travis now uses 14.04 instead of 12.04

**1.1.1**
  * IIO 4.6 kernel matrix support
  * Intel Joule radio led support
  * mraa_init_io() examples
  * MRAAPLATFORMFORCE fixes
  * fix python documentation

**1.1.0**
  * build python2 & python3 bindings
  * Intel Joule support
  * mraa_init_io() generic funtion
  * mraa-gpio fixes
  * edison PWM 0% improvements
  * iio location maping fix

**1.0.0**
  * Change API to enhance error checking in C. Changes include:
    - mraa_aio_read
    - mraa_i2c_read_byte
    - mraa_i2c_read
    - mraa_i2c_read_byte_data
    - mraa_i2c_read_word_data
    - mraa_spi_write_word
    - mraa::Spi:write_word becomes mraa::Spi::writeWord
  * Remove MRAA_ERROR_PLATFORM_ALREADY_INITIALISED which was deprecated for C89 compatibility
  * Remove pwm config_* apis as unused
  * Add mraa_remove_subplatform
  * Support NPM for android
  * Fix AIO always returning channel 0 with firmata support

**0.10.1**
  * Remove imraa from node package

**0.10.0**
  * Add Firmata support with -DFIRMATA=ON
  * Add imraa with support for pinmuxing and arduino101 flashing
  * Add better support for NVM installed node via NODE_ROOT_DIR.
  * Mraa bindings now link to libmraa.so.x
  * mraa_init() now returns MRAA_SUCCESS if platform is already initialize
  * Mraa java bindings now support JNI 1.6+
  * Added support for a02082 rpi2
  * Mraa has a logo!

**0.9.6**
  * ftdi ft4222 now dynamically loaded
  * FT4222 improved ISR code

**0.9.5**
  * Add support for the UP board (up-board.org)
  * Support OpenJDK 7 as well as OpenJDK 8
  * 96board better pin numbering
  * node v4 improvements
  * Support for PCA9555 i2c expander read/write on FT4222

**0.9.4**
  * Fix issue with edison ADC initilisation
  * documentation update

**0.9.3**
  * Intel edison detection much improved
  * ftdi ft4222 interrupt improvement
  * small fixes to java lib

**0.9.2**
  * Fix bug with mraa_init & USBPLAT
  * Add mraa_gpio_read_dir call
  * Enable cherryhill/braswell platform
  * Improved 96 board support

**0.9.1**
  * Minnowboard Turbot support
  * Change to java runables for ISR support
  * New platform version call
  * Default i2c bus on edison miniboard is now i2c-1
  * More compatibility with node.js versions > 0.10
  * New C++ get i2c default bus call
  * Various bug fixes & optimisations

**0.9.0**
  * Beta iio API in C & C++ (no SWIG support)
  * Added 96Board support
  * Added Brillo/Android support
  * Java ISR fixes
  * FT4222 gpio support

**0.8.1**
  * Nodejs 4.1.1 support
  * Java examples fixes
  * Nodejs SPI tweaks
  * Misc fixes

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
