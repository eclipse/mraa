Building libmraa                         {#building}
===============

libmraa uses cmake in order to make compilation relatively painless. Cmake runs
build out of tree so the recommended way is to clone from git and make a build/ directory.

You'll need swig version 3.0.2 and we recommend 3.0.3 to build the javascript
modules. If you're version of SWIG is older than this then please see below for
disabling SWIGNODE otherwise you will get a weird build failure.

The basic build steps are as follow, we'll assume you're building from git,
note that if you compile with git installed your version of mraa will be tagged
-dirty. This simply means git wasn't installed or that you where building form
a tarball. You can modify build/src/version.c before running make if this is
incorrect. The instructions listed here all assume that build/ is an empty dir
that lives inside the cloned repository of mraa.

~~~~~~~~~~~~~{.sh}
mkdir build
cd build
cmake ..
make
~~~~~~~~~~~~~

Our cmake configure has a number of options, cmake-gui or ccmake (cmake -i is
no longer with us :() can show you all the options. A few of the more common
ones are listed below - note that when the option starts with CMAKE_ it's an
option that is made available by cmake and will be similar in all cmake
projects. You need to add them after `cmake` but before `..`.

A few recommended options:
Changing install path from /usr/local to /usr
 -DCMAKE_INSTALL_PREFIX:PATH=/usr

Building debug build - adds -g and disables optimisations - this will force a full rebuild:
 -DCMAKE_BUILD_TYPE=DEBUG

Using clang instead of gcc:
 -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang

Building with an older version of swig (swig < 3.0.2) requires the disabling of javascript:
 -DBUILDSWIGNODE=OFF

Disabling python module building
 -DBUILDSWIGPYTHON=OFF

Building doc, this will require sphinx & doxygen
 -BUILDDOC=ON

Using a yocto/oe toolchain
--------------------------

In order to compile with a yocto/oe toolchain use the following toolchain file.
This works well on the edison 1.6 SDK. First source the environment file, then
use our cmake toolchain file.

~~~~~~~~~~~~~{.sh}
source /opt/poky-edison/1.6/environment-setup-core2-32-poky-linux
mkdir build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/oe-sdk_cross.cmake ..
make
~~~~~~~~~~~~~

Using coverity
--------------

Static analysis is routinely performed using coverity on libmraa's codebase.
This is the procedure to submit a build to coverity. You'll need to install
coverity-submit for your OS.

~~~~~~~~~~~~~{.sh}
mkdir covbuild/ && cd covbuild
cmake -DBUILDDOC=OFF -DBUILDSWIG=OFF ..
cov-build --dir cov-int make
tar caf mraa.tar.bz2 cov-int
~~~~~~~~~~~~~
