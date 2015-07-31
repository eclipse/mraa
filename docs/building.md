Building libmraa                         {#building}
===============

libmraa uses cmake in order to make compilation relatively painless. Cmake runs
build out of tree so the recommended way is to clone from git and make a build/
directory inside the clone directory.

## Build dependencies
Not all these are required but if you're unsure of what you're doing this is
what you'll need:
* [SWIG](http://swig.org) 3.0.5+
* [git](http://git-scm.com)
* [python](http://python.org) 2.7 or 3.4+ (you'll need not just the interpreter but python-dev)
* [node.js](http://nodejs.org) 0.10.x or 0.12.x (you'll need not just the interpreter but nodejs-dev)
* [CMake](http://cmake.org) 2.8.8+

To build the documentation you'll also need:
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/) 1.8.9.1+
* [Graphviz](http://graphviz.org/) 2+ (For doxygen graph generation)
* [Sphinx](http://sphinx-doc.org/) 1.1.3+ (For Python docs)


## Basic build steps

~~~~~~~~~~~~~{.sh}
mkdir build
cd build
cmake ..
make
~~~~~~~~~~~~~

If this goes wrong and you have all the dependencies installed, then please
file an issue with the full output of `cmake ..` and `make` or however far you
got.

## Configuration flags

Our cmake configure has a number of options, cmake-gui or ccmake (cmake -i is
no longer with us :() can show you all the options. A few of the more common
ones are listed below. Note that when the option starts with CMAKE_ it's an
option that is made available by cmake and will be similar in all cmake
projects. You need to add them after `cmake` but before `..`.

A few recommended options:
Changing install path from /usr/local to /usr:
 -DCMAKE_INSTALL_PREFIX:PATH=/usr

Building debug build - adds -g and disables optimisations - this will force a
full rebuild:
 -DCMAKE_BUILD_TYPE=DEBUG

Using clang instead of gcc:
 -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++

Building with an older version of SWIG (< 3.0.2) requires the disabling of javascript:
 -DBUILDSWIGNODE=OFF

Disabling python module building:
 -DBUILDSWIGPYTHON=OFF

Building doc, this will require [SPHINX](http://sphinx-doc.org) &
[Doxygen](http://doxygen.org):
 -DBUILDDOC=ON

Building with python3 (careful you need to clear cmake cache between python
version switches!)
 -DBUILDPYTHON3=ON

Override build architecture (this is useful because on x86 arm code is not
compiled so use this flag to force the target arch)
 -DBUILDARCH=arm

## Dependencies continued

You'll need at least SWIG version 3.0.2 and we recommend 3.0.5 to build the
javascript & python modules. If your version of SWIG is older than this then
please see above for disabling SWIGNODE. Otherwise you will get a weird build
failure when building the javascript module. The python module builds with SWIG
2.x.

During the build, we'll assume you're building from git, note that if you
compile with git installed your version of mraa will be tagged -dirty. This
simply means git wasn't installed or that you where building form a tarball.
You can modify build/src/version.c before running make if this is incorrect.
The instructions listed here all assume that build/ is an empty dir that lives
inside the cloned repository of mraa.

If you have multiple versions of python then mraa can get confused, we
recommend using virtualenv to select which version of python you want. We test
2.7 the most but SWIG will generate valid 3.x python code but we do not
generally support building both at once.

## Using a yocto/oe toolchain

In order to compile with a yocto/oe toolchain use the following toolchain file.
This works well on the edison 1.6 SDK. First source the environment file, then
use our cmake toolchain file.

~~~~~~~~~~~~~{.sh}
source /opt/poky-edison/1.6/environment-setup-core2-32-poky-linux
mkdir build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/oe-sdk_cross.cmake ..
make
~~~~~~~~~~~~~

## Using coverity

Static analysis is routinely performed using coverity on libmraa's codebase.
This is the procedure to submit a build to coverity. You'll need to install
coverity-submit for your OS.

~~~~~~~~~~~~~{.sh}
mkdir covbuild/ && cd covbuild
cmake -DBUILDDOC=OFF -DBUILDSWIG=OFF ..
cov-build --dir cov-int make
tar caf mraa.tar.bz2 cov-int
~~~~~~~~~~~~~

## Building Java bindings
Have JAVA_HOME set to JDK install directory. Most distributions set this from /etc/profile.d/ and have a way of switching between alternatives. We support both OpenJDK and Oracle's JDK. On Arch Linux with openjdk8 you'll have to set this yourself like this:
~~~~~~~~~~~~~{.sh}
export JAVA_HOME=/usr/lib/jvm/default/
~~~~~~~~~~~~~
Then use the cmake configuration flag:
 -DBUILDSWIGJAVA=ON
To compile Example.java
~~~~~~~~~~~~~{.sh}
javac -cp $DIR_WHERE_YOU_INSTALLED_MRAA/mraa.jar:. Example.java
~~~~~~~~~~~~~
To run, make sure libmraajava.so is in LD_LIBRARY_PATH
 ~~~~~~~~~~~~~{.sh}
jave -cp $DIR_WHERE_YOU_INSTALLED_MRAA/mraa.jar:. Example
~~~~~~~~~~~~~

## Building an IPK/RPM package using cpack

You can get cpack to generate an IPK or RPM package fairly easily if you have
the correct packaging tools

~~~~~~~~~~~~~{.sh}
cmake -DIPK=ON -DCMAKE_INSTAL_PREFIX=/usr ..
make package
~~~~~~~~~~~~~

To use RPM simply enable the RPM option. You'll need rpmbuild installed on your
build machine.

~~~~~~~~~~~~~{.sh}
cmake -DRPM=ON -DCMAKE_INSTAL_PREFIX=/usr ..
~~~~~~~~~~~~~
