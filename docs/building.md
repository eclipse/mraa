Building libmraa                         {#building}
===============
libmraa uses cmake in order to make compilation relatively painless. CMake runs
build out of tree so the recommended way is to clone from git and make a `build/`
directory inside the clone directory.

For building imraa check @ref buildingimraa page.
## Build dependencies
Not all these are required but if you're unsure of what you're doing this is
what you'll need:
* [SWIG](http://swig.org) 3.0.5+
* [git](http://git-scm.com)
* [python](http://python.org) 2.7 or 3.4+ (you'll need not just the interpreter but python-dev)
* [node.js](http://nodejs.org) 4.x recommended (you'll need not just the interpreter but nodejs-dev)
* [CMake](http://cmake.org) 2.8.8+ (3.1+ is recommended for node.js version 2+)
* [json-c](https://github.com/json-c/json-c) 0.12+ (0.10+ probably works in reality)

For Debian-like distros the below command installs the basic set:

```bash
sudo apt-get install git build-essential swig3.0 python-dev nodejs-dev cmake libjson-c-dev
```

To build the documentation you'll also need:
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/) 1.8.9.1+
* [Graphviz](http://graphviz.org/) 2+ (For Doxygen graph generation)
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

After that you can install built files (into default path) by running:


```bash
sudo make install
```

See flags for adjusting install paths in the section below.

Currently our install logic puts Python bindings into standard paths, which
do not work on Debian due to their
 [policy](http://www.debian.org/doc/packaging-manuals/python-policy/ch-python.html#s-paths).

We are working on a permanent solution, in the meantime please use this command
after `make install` to link installed modules where Debian's Python expects
them:

```bash
sudo ln -s <your install prefix, e.g. /usr>/lib/python2.7/site-packages/* /usr/lib/python2.7/dist-packages
```

Same approach works for Python 3, you'll just need to adjust the version number
in the path accordingly.

## Configuration flags

Our CMake configuration has a number of options, `cmake-gui` or `ccmake` (`cmake -i` is
no longer with us :() can show you all the options. A few of the more common
ones are listed below. Note that when the option starts with `CMAKE_` it's an
option that is made available by CMake and will be similar in all CMake
projects. You need to add them after `cmake` but before `..`

A few recommended options:

Changing install path from `/usr/local` to `/usr`:
 `-DCMAKE_INSTALL_PREFIX:PATH=/usr`

Building debug build - adds `-g` and disables optimisations - this will force a
full rebuild:
 `-DCMAKE_BUILD_TYPE=DEBUG`

Using `clang` instead of `gcc`:
 `-DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++`

Building with an older version of SWIG (< 3.0.2) requires the disabling of JavaScript:
 `-DBUILDSWIGNODE=OFF`

Disabling Python module building:
 `-DBUILDSWIGPYTHON=OFF`

Building doc, this will require [SPHINX](http://sphinx-doc.org) &
[Doxygen](http://doxygen.org):
 `-DBUILDDOC=ON`
You will also require clone git submodules from your existing checkout:
 `git submodule update --init --recursive`
Then from doxygen2jsdoc dir:
 `npm install`
Then from doxyport dir:
 `make setup`

Override build architecture (this is useful because on x86 ARM code is not
compiled so use this flag to force the target arch)
 `-DBUILDARCH=arm`

You can also enable -Wall for gcc before running cmake by exporting your wanted
CC flags to the CC env var
  `export CC="gcc -Wall"`

Sometimes it's nice to build a static libary, on Linux systems just set
   `-DBUILD_SHARED_LIBS=OFF`

## Dependencies continued

You'll need at least SWIG version 3.0.2 and we recommend 3.0.5 to build the
JavaScript & Python modules. If your version of SWIG is older than this then
please see above for disabling `SWIGNODE`. Otherwise you will get a weird build
failure when building the JavaScript module. The Python module builds with SWIG
2.x but we don't test it.

During the build, we'll assume you're building from git, note that if you
compile with `git` installed your version of mraa will be versioned with `git
desribe --tag` to make it easy for intentification. You can easily modify
version.c in build/src. If you don't build from a git tree then you will simply
have a version which matches the latest released version of mraa.

## Using a Yocto/OE toolchain

In order to compile with a Yocto/OE toolchain use the following toolchain file.
This works well on the Edison 1.7.2 SDK. First source the environment file, then
use our CMake toolchain file.

~~~~~~~~~~~~~{.sh}
source /opt/poky-edison/1.7.2/environment-setup-core2-32-poky-linux
mkdir build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/oe-sdk_cross.cmake ..
make
~~~~~~~~~~~~~

## Using Coverity

This is the procedure to submit a build to Coverity. You'll need to install
`coverity-submit` for your OS.

~~~~~~~~~~~~~{.sh}
mkdir covbuild/ && cd covbuild
cmake -DBUILDDOC=OFF -DBUILDSWIG=OFF ..
cov-build --dir cov-int make
tar caf mraa.tar.bz2 cov-int
~~~~~~~~~~~~~

## Building Java bindings
Have JAVA_HOME set to JDK install directory. Most distributions set this from `/etc/profile.d/`
 and have a way of switching between alternatives. We support both OpenJDK and Oracle's JDK.
 On Arch Linux with OpenJDK 8 you'll have to set this yourself like this:
~~~~~~~~~~~~~{.sh}
export JAVA_HOME=/usr/lib/jvm/default/
~~~~~~~~~~~~~
Then use the CMake configuration flag:
 `-DBUILDSWIGJAVA=ON`
To compile `Example.java`
~~~~~~~~~~~~~{.sh}
javac -cp $DIR_WHERE_YOU_INSTALLED_MRAA/mraa.jar:. Example.java
~~~~~~~~~~~~~
To run, make sure `libmraajava.so` is in `LD_LIBRARY_PATH`
 ~~~~~~~~~~~~~{.sh}
jave -cp $DIR_WHERE_YOU_INSTALLED_MRAA/mraa.jar:. Example
~~~~~~~~~~~~~

If you want to add or improve Java bindings for mraa, please follow the <a href="https://github.com/intel-iot-devkit/upm/blob/master/docs/creating_java_bindings.md">Creating Java Bindings Guide</a>.

## Building an IPK/RPM package using `cpack`

You can get `cpack` to generate an IPK or RPM package fairly easily if you have
the correct packaging tools

~~~~~~~~~~~~~{.sh}
cmake -DIPK=ON -DCMAKE_INSTALL_PREFIX=/usr ..
make package
~~~~~~~~~~~~~

To use RPM simply enable the RPM option. You'll need `rpmbuild` installed on your
build machine.

~~~~~~~~~~~~~{.sh}
cmake -DRPM=ON -DCMAKE_INSTALL_PREFIX=/usr ..
~~~~~~~~~~~~~

## Building for the Android Things Peripheralmanager Client

Requirements:
* Android [Things Native Library](https://github.com/androidthings/native-libandroidthings)
* Android NDK >= 14b

The [Things Native Library](https://github.com/androidthings/native-libandroidthings) contains a CMake find_package module
[FindAndroidThings.cmake](https://github.com/androidthings/native-libandroidthings/blob/master/FindAndroidThings.cmake). Make sure the directory containing this module is
added to the CMAKE_MODULE_PATH.

### NDK r14b

~~~~~~~~~~~~~{.sh}
cmake -DBUILDSWIG=OFF -DBUILDARCH=PERIPHERALMAN -DANDROID_TOOLCHAIN_NAME=x86-i686 -DCMAKE_TOOLCHAIN_FILE=/path/to/android-ndk-r14b/build/cmake/android.toolchain.cmake -DCMAKE_MODULE_PATH=/path/to/native-libandroidthings ..
~~~~~~~~~~~~~

## Building with Docker

You can use `docker` and `docker-compose` to generate a complete build environment
for mraa without having to install any other tool.

Requirements:
* [docker](https://www.docker.com/get-docker) >= 1.12.6
* [docker-compose](https://docs.docker.com/compose/install/) >= 1.9.0

**NOTE:** docker-compose is an optional requirement. It actually make running complex
docker build and run command easier. But you can just use docker to build and run.

### Using Docker Images to build Mraa

**tl;dr:** Just use this commands to build mraa:

```sh
# Build mraa documentation
$ docker-compose run doc
# Build mraa python2 package and run python2 tests
$ docker-compose run python2
# Build mraa python3 package and run python3 tests
$ docker-compose run python3
# Build mraa java package and run java tests
$ docker-compose run java
# Build mraa node4 package
$ docker-compose run node4
# Build mraa node5 package
$ docker-compose run node5
# Build mraa node6 package
$ docker-compose run node6
# Build mraa for android things package
$ docker-compose run android
# Run Sonar Qube Scans for mraa
$ docker-compose run sonar-scan
```

**docker-compose** will take a look at the `docker-compose.yaml` file in the repository
root directory, pull the required docker image, and run an specific command to build
mraa for the requested target.
Once the build is completed, you will have a `build/` folder in the repository root with all
the compiled code. This `build/` folder is created by using a docker volume. The `build\`
folder contents is reused each time you execute `docker-compose run [TARGET]`.
To know more about volumes in Docker, visit the [Docker Volume Documentation](https://docs.docker.com/engine/tutorials/dockervolumes/).

You can also start an interactive session inside the docker container if you need to run some
custom build commands:

```sh
# Start an interactive bash  shell inside the container
$ docker-compose run python2 bash
# From now, all the commands are executed inside the container
$ cd build && cmake -DBUILDSWIGPYTHON=ON .. && make clean all
```

If you don't want to use docker-compose, you can also use `docker run` to build mraa.
For example, to build mraa for python2, you can do:

```sh
# From the repository root folder
$ docker run \
      --volume=$(pwd):/usr/src/app \
      --env BUILDSWIG=ON \
      --env BUILDSWIGPYTHON=ON \
      --env BUILDSWIGJAVA=OFF \
      --env BUILDSWIGNODE=OFF \
      dnoliver/mraa-python \
      bash -c "./scripts/run-cmake.sh && make -Cbuild _python2-mraa"
```

### Proxy considerations

If, for some reason, you are behind a proxy, find below a list of common problems related
to proxy settings:

**docker cannot pull images from docker.io**

 Visit [this link](https://docs.docker.com/engine/admin/systemd/#httphttps-proxy)
 to configure docker daemon behind a proxy.

**docker run fails to access the internet**

docker-compose will automatically take `http_proxy`, `https_proxy`, and `no_proxy`
environment variables and use it as build arguments. Be sure to properly configure
this variables before building.

docker, unlinke docker-compose, do not take the proxy settings from the environment
automatically. You need to send them as environment arguments:

```sh
# From the repository root folder
$ docker run \
    --volume=$(pwd):/usr/src/app \
    --env BUILDSWIG=ON \
    --env BUILDSWIGPYTHON=ON \
    --env BUILDSWIGJAVA=OFF \
    --env BUILDSWIGNODE=OFF \
    --env http_proxy=$http_proxy \
    --env https_proxy=$https_proxy \
    --env no_proxy=$no_proxy \
    dnoliver/mraa-python \
    bash -c "./scripts/run-cmake.sh && make -Cbuild _python2-mraa"
```
