libmraa Internals                       {#internals}
=================

For building see @ref building. This will describe the general internal build
of libmraa and will be useful to developers who'd like to understand more of
how libmraa works or who'd like to add additional platforms. The internals will
deal with the C API as that is the low level API which libmraa is built around.
Note that C++ is simply a header only wrapper of the C API.

libmraa has the philosophy that the board mapping is what we typically use in
the API with the execption of i2c/spi bus numbering as they are typically not
labelled on boards and so we use the kernel numbering scheme. Whilst this can
confuse some, it's typically not an issue as platforms rarely expose more than
one of these for user use and so when this is the case, libmraa will always use
the bus in the pinmapper. For example edison uses i2c #6 but since there is
only one, libmraa will try to be helpful and everything is treated as 6 when
doing a mraa_i2c_init(). The _raw functions will override the pinmapper and can
be accessed without a valid board configuration. This can be helpful either in
development of platform configurations for mraa or when modifying kernels
etc... Internally the mechanism is used heavily.

In libmraa, all code is split into 7 modules, src/{i2c, spi, gpio, uart, pwm,
aio and common}. These should be fairly self explanatory in goals/purpose but a
few work in different ways. Public APIs are stored in api/ and internal headers
are in include/

### Logging ###

Logging is now done purely in syslog(). Note that on platforms running systemd
journald will intercept syslog(3) calls and log to the journal instead. You can
set the log mask by using mraa_set_log_level(). Doing a DEBUG build of libmraa
will also cause the DEBUG macro to be defined which will cause the syslog mask
to be unset.

### Contexts ###

libmraa uses contexts to store all information, this context cannot be accessed
by the user and so it's layout can and may be changed without warning to users.
If an init() function fails it will return NULL and further calls with this
context will lead to undefined behaviour.

### Pinmapper ###

The mraa_board_t is defined in mraa/common.h. It's a mostly static structure
initialised during mraa_init(). The pinmap file in
src/{arch}/{manufacturer}_{boardname}_{revision}.c then fills this array. It's
also where platform hooks can be defined, functions that will be run at various
'hook' points in the code.

The mraa_pininfo_t structure needs to be set for the board pincount (set in a
macro in the platform configuration header. Every pin will have a
mraa_pincapabilities_t which will define what it can do. The doxygen
documentation explains how this works but it's essentially a bitfield which
needs to be set for every capability the pin can have. Gpios can have multiple
muxes which will be set at the gpio init before it can be toggled.

### i2c ###

I2c from userspace in GNU/Linux is handled by character devices handled by the
kernel driver i2c-dev. For more details the i2c/dev-interface documentation
file in the kernel is the place to go.

In libmraa, we re-use part of a library - libi2c from RoadNarrows -
i2c/smbus.c. This library simply makes it easier for us to handle the error
conditions that can arrise when writing on i2c buses. Essentially the API is
fairly simple consisting of writes & reads.

Careful - on alot of platforms i2cdetect will often crash. To findi your i2c
addresses please look at your sensor's datasheet! If using i2cdetect most
platforms do not support SMBus quick write so use the '-r' flag.

### spi ###

Mraa deals exclusively with spidev, so when we say bus we really mean bus +
chip select from spidev. Spi(0) could lead to spidev5.1 and Spi(1) to
spidev5.2. Typically on a micro using a random gpio as a chip select works
well, and on some platforms if one is careful with threads this can work well
with mraa. However when a kernel module shares the same bus as spidev (but on a
different CS) this behaviour is *very* dangerous. Platforms such as Galileo
Gen2 & Edison + Arduino breakout board work this way. Mraa will not help you in
using a non hardware chip select, do so at your own peril!

### gpio ###

GPIO is probably the most complicated and odd module in libmraa. It is based on
the gpiolib kernel driver framework which uses sysfs. There is a lot of good
documentation in gpio/sysfs.txt in the kernel docs.

The main issue is that gpios on hobbyist boards typically come with a number of
muxers or level shifters and are often mapped in crazy ways. libmraa's goal is
to make the label on your board match the API :) We hope that pleases you.

Because boards are very different we use alot of platform hooks (@ref hooks) to
make the initialisation work on all platforms. The hope is that simple
platforms with no level shifters or expanders will work with just the pinmap
definition.

GPIOs are typically interfaced via sysfs because that's easier for us but we
can also work with fast gpio. This is typically preffered to do mmap gpio
access. This is however trickier and typically relies on lots of platform
hooks. By default we support hitting /dev/mem or another device at specific
addresses to toggle gpios which is how mmap access works on some boards.

Note that in Linux gpios are numbered from ARCH_NR_GPIOS down. This means that
if ARCH_NR_GPIOS is changed, the gpio numbering will change. In 3.18+ the
default changed from 256 to 512, sadly the value cannot be viewed from
userspace so we rely on the kernel version to extrapolate the likely value.

### uart ###

libmraa does not support UART/serial as there are many good libraries that do
this already. In the future we may wrap or use one. However the class exists to
set the pinmapper correctly for uart to work on some platforms.

### pwm ###

Internally everything with PWM in mraa is in microseconds because that's what
the linux kernel uses and is probably all the granularity we'll ever
need/achieve with the kind of hardware we're targetting. Board configuration
pwm max/min values are always set in microseconds.

### aio ###

AIO pins are numbered after GPIO pins. This means that on Arduino style boards
pin 14 is A0. Typically mraa will only support an ADC if a platform ships with
one and has a good kernel module for it. Extra i2c/spi ADCs can be supported
via something like UPM but are unlikely to receive support in mraa at the moment.

Note that giving mraa_aio_init(0) will literally query the pinmapper for
board->gpio_count + 0 so you must place your aio pins after gpio_count. This is
the default behaviour but can of course be overriden by advance function
pointers. Whilst maybe not the sanest of defaults, most of the hobbyist boards
we deal with follow a naming pattern similar to Arduino or have no ADC so for
now we have considered this sensible.

### Initialisation ###

mraa_init() needs to be called in order to initialise the platform files or
'pinmap'. Because calling this is tedious libmraa uses a C constructor to run
mraa_init on library load. This means that it is not possible to stop this
running and all function calls like mraa_set_log_level() will not work during
mraa_init(). This feature is supported by most sane compilers and libcs but you
can turn off CTORS in uclibc, though I've yet to find a configuration with
someone doing that. mraa_init() can be called multiple times if you feel like
being 'safe'.

In the SWIG modules mraa_init() is called during the %init stage of the module
loading. This is simply to avoid mraa_init() running 'too' early, though I've
never seen an issue in running it in a CTOR.

### SWIG ###

At the time when libmraa was created (still the case?) the only - working -
API/wrapper generation tool that supported nodejs was SWIG. For more general
information on SWIG please see the SWIG documentation.

The src/{javascript, python} & src/mraa.i folders contain all the files for the
SWIG generation. The C++ headers in api/mraa/ are given as input sources to
SWIG. SWIG modules do not link to libmraa (although maybe that would be a good
idea...)

Typemaps are used heavily to map uint8_t* pointers to bytearrays and
node_buffers. These are native python & node.js types that represent uint8_t
data the best and are very well supported in both languages. Argument
conversions and memory allocations are performed so the performance of using
these functions compared to the C/C++ equivalent will likely be a little lower,
however it is much more natural than using carrays.i typemap library.

### NPM ###

mraa is published on NPM, there is a target to prebuild a mraa src tarball that
can be built with node-gyp. The way this works is to use the mraa_LIB_SRCS
array to generate a binding.gyp file from the skeleton binding.gyp.cmake in
src/javascript. Because we don't expect most NPM users to have SWIG we
precompile the src/mraajsJAVASCRIPT_wrap.cxx. The src/version.c is already
known since this is a static tarball so we write that too. These files are
placed not in a build/ directory but in the main mraa directory. You can then
tar the directory up and send it to NPM. This is done automatically on every
commit by our automated build system.

