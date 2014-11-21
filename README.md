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

* [Galileo Gen 1 - Rev D](../master/docs/galileorevd.md)
* [Galileo Gen 2 - Rev H](../master/docs/galileorevh.md)
* [Edison](../master/docs/edison.md)
* [Intel DE3815](../master/docs/intel_de3815.md)
* [Minnowboard Max](../master/docs/minnow_max.md)

Installing on your board
========

See the section below on compiling or use our repository to install on an
eglibc based yocto poky image that supports opkg. Adding this repository is as
simple as:

``` bash
echo "src mraa-upm http://iotdk.intel.com/repos/1.1/intelgalactic" > /etc/opkg/mraa-upm.conf
opkg update
opkg install libmraa0
```

Compiling
=========

See documentation on [building](../master/docs/building.md)

Examples
========

See the [examples](../../tree/master/examples) available for various languages

C/C++ API Documentation
===========

The C/C++ documentation is available [here](http://iotdk.intel.com/docs/master/mraa/)

Python API Documentation
===========

The Python documentation is available [here](http://iotdk.intel.com/docs/master/mraa/python)

NodeJS API documentation
===========

There is no explicit nodejs API documentation, see the examples. The API itself
is very similar to the python one but with a js syntax. If you know of an
autodoc tool for generating nodejs APIs from a native C++ nodejs extension
please get in touch!
