MRAA - Low Level Skeleton Library for Communication on GNU/Linux platforms
==============

Library in C/C++ to interface with Galileo & other Intel platforms, in a
structured and sane API with port names/numbering that match boards & with
bindings to javascript & python.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

Supported Boards
================

* [Galileo Rev D](../master/docs/galileorevd.md)
* [Galileo Rev H](../master/docs/galileorevh.md)

Installing on your board
========

See the section below on compiling or use our repository to install on an
eglibc based yocto poky image that supports opkg. Adding this repository is as
simple as:

``` bash
echo "src mraa-upm http://iotdk.intel.com/repos/1.1/intelgalactic" > /etc/opkg/mraa-upm.conf
opkg update
opkg install mraa
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
