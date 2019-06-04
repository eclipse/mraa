<p align="center">
  <img src="http://iotdk.intel.com/misc/logos/mraa.png" height="150px" width="auto" algt="Mraa Logo"/>
</p>

Eclipse Mraa - Low Level Skeleton Library for Communication on GNU/Linux platforms
=============================================================================

Eclipse Mraa (Libmraa) is a C/C++ library with bindings to Java, Python and JavaScript to
interface with the IO on Galileo, Edison & other platforms, with a structured
and sane API where port names/numbering matches the board that you are on. Use
of libmraa does not tie you to specific hardware with board detection done at
runtime you can create portable code that will work across the supported
platforms.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

The MRAA project is joining the Eclipse Foundation as an Eclipse IoT project.
You can read more about this [here](https://projects.eclipse.org/proposals/eclipse-mraa).

[![Build Status](https://travis-ci.org/intel-iot-devkit/mraa.svg?branch=master)](https://travis-ci.org/intel-iot-devkit/mraa) [![Quality Gate](https://sonarcloud.io/api/project_badges/measure?project=mraa-master&metric=alert_status)](https://sonarcloud.io/dashboard?id=mraa-master)

Supported Boards
================

X86
---
* [Galileo Gen 1 - Rev D](../master/docs/galileorevd.md)
* [Galileo Gen 2 - Rev H](../master/docs/galileorevh.md)
* [Edison](../master/docs/edison.md)
* [Intel DE3815](../master/docs/intel_de3815.md)
* [Minnowboard](../master/docs/minnow_max.md)
* [NUC 5th generation](../master/docs/intel_nuc5.md)
* [UP](../master/docs/up.md)
* [UP Squared](../master/docs/up2.md)
* [Intel Joule](../master/docs/joule.md)
* [IEI Tank](../master/docs/iei-tank.md)
* [ADLINK IPi-SMARC X86](../master/docs/adlink_ipi_x86.md)
* [UP Xtreme](../master/docs/up_xtreme.md)

ARM
---
* [Raspberry Pi](../master/docs/raspberry_pi.md)
* [Banana Pi](../master/docs/banana_pi.md)
* [Beaglebone Black](../master/docs/beaglebone.md)
* [phyBOARD-Wega](../master/docs/phyboard-wega.md)
* [96Boards](../master/docs/96boards.md)
* [ADLINK IPi-SMARC ARM](../master/docs/adlink_ipi_arm.md)
* [Rock Pi 4](../master/docs/rockpi4.md)

MIPS
---
* [Linkit Smart 7688](../master/docs/linkit_7688.md)
* [Onion Omega2](../master/docs/omega2.md)

FPGA
----
* [DE10-Nano](../master/docs/de_nano_soc.md)

USB
---
* [FT4222](../master/docs/ftdi_ft4222.md)
* [Firmata](../master/docs/firmata.md)

I2C
---
* [GrovePi](../master/docs/grovepi.md)

Mock
----
* [Generic simulated board](../master/docs/mock.md)

JSON platform
----
* [Platform loading](../master/docs/jsonplatform.md)

Installing on your board
========================

Installing on Ubuntu
--------------------

Here is a PPA for installing on Ubuntu Xenial or Bionic:
https://launchpad.net/~mraa/+archive/ubuntu/mraa

```bash
sudo add-apt-repository ppa:mraa/mraa
sudo apt-get update
sudo apt-get install libmraa2 libmraa-dev libmraa-java python-mraa python3-mraa node-mraa mraa-tools
```

Running MRAA tools or applications on Ubuntu systems requires elevated permissions
(e.g. run with `sudo`).

Install on Arch Linux
---------------------

There is an AUR package for mraa here: https://aur.archlinux.org/packages/mraa

Install on openSUSE
-------------------

```bash
REPO="openSUSE_Tumbleweed"
if test "$(arch)" == "aarch64"; then
  REPO="openSUSE_Factory_ARM"
fi
sudo zypper ar http://download.opensuse.org/repositories/hardware/$REPO/hardware.repo
sudo zypper in mraa
```

Install on Fedora Linux
-----------------------

There is an mraa package in the main Fedora repository so it can be dnf installed
in all recent Fedora releases. The Node.js and Python 3 bindings are packaged as
separate packages.

```bash
sudo dnf install mraa nodejs-mraa python3-mraa
```

Installing for Node.js only
---------------------------

> Note: Node.js 7.0.0+ is not currently supported unless compiling with a patched
vesion of SWIG. See the corresponding section and document below.

You can also install just the node.js mraa module by using npm. You will need a
C++ compiler and the node development headers, however it's not required to
have SWIG installed. This works for node versions 6.x.x and prior.

```bash
npm install mraa
```

Note that installing mraa in this way builds mraa without json-c so you cannot
use mraa_init_json_platform(). Also building this way means the mraa.node
includes a static version of libmraa rather than relying on a dynamic library
in /usr/lib.

Subplatforms (i.e. Firmata) have to be added manually with this kind of install
from your application, as shown in [this example](examples/javascript/firmata.js).

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
