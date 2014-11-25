Building libmraa                         {#building}
===============

libmraa uses cmake in order to make compilation relatively painless. Cmake runs
build out of tree so the recommended way is to clone from git and make a build/ directory.

You'll need swig version 3.0.1+, currently I'm using
8d226e39dc7a958013ff9ffd15e231ef206ba265 from the swig master branch. This is
only required for javascript support and can be disabled with a cmake define
(see below).

~~~~~~~~~~~~~{.sh}
mkdir build
cd build
cmake ..
make
~~~~~~~~~~~~~

Our cmake configure has a number of options, cmake-gui or ccmake can show you
all the options. The interesting ones are detailed below:

Few recommended options:
Changing install path from /usr/local to /usr
 -DCMAKE_INSTALL_PREFIX:PATH=/usr

Building debug build:
 -DCMAKE_BUILD_TYPE=DEBUG

Using clang instead of gcc:
 -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang

Building with an older version of swig (swig 2.0+) requires the disabling of javascript:
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
