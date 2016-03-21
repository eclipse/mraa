<p align="center">
  <img src="http://iotdk.intel.com/misc/logos/mraa.png" height="150px" width="auto" algt="Mraa Logo"/>
</p>
libmraa - Low Level Skeleton Library for Communication on GNU/Linux platforms
==============

Libmraa is a C/C++ library with bindings to javascript & python to interface
with the IO on Galileo, Edison & other platforms, with a structured and sane
API where port names/numbering matches the board that you are on. Use of
libmraa does not tie you to specific hardware with board detection done at
runtime you can create portable code that will work across the supported
platforms.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

Supported Boards
================

X86
---
* [Galileo Gen 1 - Rev D](../master/docs/galileorevd.md)
* [Galileo Gen 2 - Rev H](../master/docs/galileorevh.md)
* [Edison](../master/docs/edison.md)
* [Intel DE3815](../master/docs/intel_de3815.md)
* [Minnowboard Max](../master/docs/minnow_max.md)
* [NUC 5th generation](../master/docs/intel_nuc5.md)
* [UP](../master/docs/up.md)

ARM
---
* [Raspberry Pi](../master/docs/raspberry_pi.md)
* [Bannana Pi](../master/docs/banana_pi.md)
* [Beaglebone Black](../master/docs/beaglebone.md)

USB
---
* [FT4222](../master/docs/ftdi_ft4222.md)

Installing on your board
========

See the section below on compiling or use our repository to install on a glibc
based yocto poky image that supports opkg. Adding this repository is as simple
as and you'll have the latest stable tagged build of mraa installed!

``` bash
echo "src mraa-upm http://iotdk.intel.com/repos/3.0/intelgalactic/opkg/i586" > /etc/opkg/mraa-upm.conf
opkg update
opkg install mraa
```

If you would like to get the latest & greatest builds from master HEAD you can
use our -dev repository

```bash
echo "src mraa-upm http://iotdk.intel.com/repos/3.0/intelgalactic-dev/opkg/i586" > /etc/opkg/mraa-upm.conf
opkg update
opkg install mraa
```

You can also install just the node.js mraa module by using npm. You will need a
C++ compiler and the node development headers, however it's not required to
have SWIG installed.

```bash
npm install mraa
```

Compiling
=========

See documentation on [building](../master/docs/building.md)

Examples
========

See the [examples](../../tree/master/examples) available for various languages

Debugging
=========

Sometimes it just doesn't want to work, let us try and help you, you can file
issues in github or join us in #mraa on freenode IRC, hang around for a little
while because we're not necessarily on 24/7, but we'll get back to you! Have a
glance at our [debugging](../master/docs/debugging.md) page too.

API Documentation
=================

<a href="http://c.mraa.io"><img src="http://iotdk.intel.com/misc/logos/c++.png"/></a>
<a href="http://java.mraa.io"><img src="http://iotdk.intel.com/misc/logos/java.png"/></a>
<a href="http://py.mraa.io"><img src="http://iotdk.intel.com/misc/logos/python.png"/></a>
<a href="http://js.mraa.io"><img src="http://iotdk.intel.com/misc/logos/node.png"/></a>

Contact Us
==========

To ask questions either file issues in github or send emails on our [mailing
list](https://lists.01.org/mailman/listinfo/mraa). You might also catch us on
the mraa channel on freenode IRC.

See the [Contribution](CONTRIBUTING.md) documentation for more details.

Changelog
=========

Version changelog [here](docs/changelog.md).
