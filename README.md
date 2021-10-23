<p align="center">
  <img src="http://iotdk.intel.com/misc/logos/mraa.png" height="150px" width="auto" algt="Mraa Logo"/>
</p>

Eclipse Mraa - Low Level I/O Communications Library for GNU/Linux platforms
===========================================================================

Eclipse Mraa (Libmraa) is a C/C++ library with bindings to Java, Python and JavaScript
to interface with the I/O pins and buses on various IoT and Edge platforms, with a
structured and sane API where port names/numbering match the board that you are on.
Use of libmraa does not tie you to specific hardware. Since board detection done at
runtime you can create portable code that will work across the supported platforms.

The intent is to make it easier for developers and sensor manufacturers to map
their sensors & actuators on top of supported hardware and to allow control of
low level communication protocol by high level languages & constructs.

The MRAA project is an Eclipse IoT project. A detailed project description can be found [here](https://projects.eclipse.org/proposals/eclipse-mraa).

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

Install on openSUSE or SLE
---------------------------

For the latest versions of openSUSE and SLE the project is distributed via
native repositories and can be installed by developers with:

```bash
sudo zypper install mraa mraa-devel
```

For ARM boards use:

```bash
REPO="openSUSE_Tumbleweed"
if test "$(arch)" == "aarch64"; then
  REPO="openSUSE_Factory_ARM"
fi
sudo zypper ar http://download.opensuse.org/repositories/hardware/$REPO/hardware.repo
sudo zypper in mraa
```

A full list of packages and instructions for installing MRAA for various openSUSE releases
can be found [here](https://software.opensuse.org/package/mraa).

Install on Fedora Linux
-----------------------

There is an mraa package in the main Fedora repository so it can be dnf installed
in all recent Fedora releases. The Node.js and Python 3 bindings are packaged as
separate packages.

```bash
sudo dnf install mraa nodejs-mraa python3-mraa
```

Installing for Red Hat Enterprise Linux, CentOS and Other Linux Distributions
---------------------------

The MRAA project does not currently distribute official binaries for RHEL
or CentOS so developers will have to compile the project from sources as
described in the next section.

For testing and development purposes it may be possible to share and install
.deb and .rpm packages across similar Linux builds.

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

To ask questions either file an issue on Github or send an email to our
[mailing list](https://accounts.eclipse.org/mailing-list/mraa-dev).
You must be subscribed to the list before you can post.
You might also catch us on the mraa channel on freenode IRC.

See the [Contribution](CONTRIBUTING.md) documentation for more details.

Changelog
=========

Version changelog [here](docs/changelog.md).
