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
doing a mraa_i2c_init and so when this is the case, libmraa will always use the
bus in the pinmapper. For example edison uses i2c #6 but since there is only
one, libmraa will try to be helpful and everything is treated as 6 when doing a
mraa_i2c_init().

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
src/{manufacturer}_{boardname}_{revision}.c then fills this array. It's also
where platform hooks can be defined, functions that will be run at various
'hook' points in the code.

The mraa_pininfo_t structure needs to be set for the board pincount (set in a
macro in the platform configuration header. Every pin will have a
mraa_pincapabilities_t which will define what it can do. The doxygen doc
explains how this works but it's essentially a bitfield which needs to be set
for every capability the pin can have. Gpios can have multiple muxes which will
be set at the gpio init before it can be toggled.

### i2c ###

I2c from userspace in GNU/Linux is handled by character devices handled by the
kernel driver i2c-dev. For more details the i2c/dev-interface documentation
file in the kernel is the place to go.

In libmraa, we re-use part of a library - libi2c from RoadNarrows -
i2c/smbus.c. This library simply makes it easier for us to handle the error
conditions that can arrise when writing on i2c buses. Essentially the API is
fairly simple consisting of writes & reads.

Careful - on alot of platforms i2cdetect will often crash, for finding your i2c
addresses please look at your sensors datasheet!

### spi ###

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
hooks. We do support by default to go hit /dev/mem or another device at
specific addresses to toggle gpios which is how mmap access works on some
boards.

### uart ###

libmraa does not support UART/serial as there are many good libraries that do
this already. In the future we may wrap or use one. However the class exists to
set the pinmapper correctly for uart to work on some platforms.

### pwm ###

### aio ###

### Initialisation ###

mraa_init() needs to be called in order to initialise the platform files or
'pinmap'. Because calling this is tedious libmraa uses a C constructor to run
mraa_init on library load. This means that it is not possible to stop this
running and all functino calls like mraa_set_log_level() will not work during
mraa_init(). This feature is supported by most sane compilers & libcs but you
can turn off CTORS in uclibc, though I've yet to find a configuration with
someone doing that. mraa_init() can be called multiple times if you feel like
being 'safe'.

In the SWIG modulse mraa_init() is called during the %init stage of the module
loading. This is simply to avoid mraa_init() running 'too' early, though I've
never seen an issue in running it in a CTOR.

### SWIG ###

At the time when libmraa was created the only - working - API/wrapper
generation tool that supported nodejs was SWIG. For more general information on
swig please see the swig documentation.

The src/{javascript, python} & src/mraa.i folders contain all the files for the
swig generation. The C++ headers in api/mraa/ are given as input sources to
SWIG. SWIG modules do not link to libmraa (although maybe that would be a good
idea...)

