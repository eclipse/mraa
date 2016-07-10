Mock platform   {#mock}
=============

Mocking capability allows libmraa user to work with the library without any real
hardware available. Enabling this requires library recompilation with architecture
override (see Building section below). When mock is enabled, library simulates
actual HW operations at the backend so that for the application it looks
like a usual board. Being implemented at the backend, the functionality is available
in all language bindings libmraa supports.

Board configuration
-------------------

This feature is yet in the experimental mode and not all functionality is available.
Right now we simulate a single generic board with GPIO (without ISR) and
an ADC with 10 (std)/12 (max) bit resolution, which returns random values on read.

We plan to develop it further and all [contributions](../CONTRIBUTING.md) are more than welcome.

See the table below for pin layout and features

| MRAA Number | Pin Name |            Notes                      |
|-------------|----------|---------------------------------------|
| 0           | GPIO0    | GPIO pin, no muxing, no ISR           |
| 1           | ADC0     | AIO pin, returns random value on read |

Building
--------

Generally all the building steps are the same as listed
in the [main building guide](./building.md), you just need to set some specific
CMake options.

### Linux

To build under Linux, follow standard instructions, just make sure to set
the `-DBUILDARCH="MOCK"` CMake option.

### Windows

Mocking capability allows us to build and use libmraa under Windows. That helps
if you e.g. don't want to leave your customary Windows-based Python IDE, but
want to develop libmraa-based programs.

Building Node.js bindings was not yet tested under Windows as MSYS2
does not have a ready-made package. Java was not tested either.

#### Prerequisites

You'll need the following to build libmraa under Windows:

* [MSYS2](http://mingw-w64.org/doku.php/download/msys2) basic installation
* Several additional packages, install them by running

  ```bash
  pacman -S cmake base-devel gcc git
  ```

#### Compiling

The procedure is conceptually the same as under Linux - you first need to run
CMake with specific options to generate makefiles and then run make to build everything.

* Run MSYS2 shell (not a MinGW one)
* Clone the libmraa git repo (let's assume into `/home/test/mraa/mraa-src` dir)
* Create a build directory outside of the clone one (let's say `/home/test/mraa/mraa-build`)
* Run CMake, switching off unsupported options and enabling mock platform:

  ```bash
  cmake ../mraa-src/ -DBUILDARCH="MOCK" -DBUILDSWIGNODE=OFF -DENABLEEXAMPLES=OFF
  ```

* Make, install and test:

  ```bash
  make clean && make install && make test
  ```

  All tests should pass.

**Note:** To have autocompletion in Python IDE, just point it to MSYS2's Python
and make sure to specify any additional paths pointing to site-packages dir
with mraa module if IDE requires that ("Interpreter Paths" in PyCharm).
With the above settings the module will be installed into `/usr/local/lib/python2.7/site-packages`
and the libmraa itself - into `/usr/local/bin`.
