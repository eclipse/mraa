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

Our cmake configure has a number of options, `cmake -i` will ask you all sorts
of interesting questions, you can disable swig modules, build documentation
etc...

Few recommended options:
Changing install path from /usr/local to /usr
-DCMAKE_INSTALL_PREFIX:PATH=/usr

Building debug build:
-DCMAKE_BUILD_TYPE=DEBUG

Using clang instead of gcc:
 -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang

Building with an older version of swig (swig 2.0+) requires the disabling of javascript:
 -DBUILDSWIGNODE=OFF
