mraa NPM pkg                        {#npmpkg}
============

What you are looking at is a tarball made after 'make npmpkg' has been run on
our build system. It contains a few prebuilt files in order to build easily
with node-gyp on most systems. Note that this is not the preffered way of
building libmraa and if you'd like to contribute, please learn how the build
system works and get your sources from git.

Any patches for mraajsJAVASCRIPT_wrap.cxx or version.c will be ignored as these
are automatically generated files. When you execute a build from node-gyp/npm
you do not generate the mraajsJAVASCRIPT_wrap.cxx file but rather use one that
we have generated for you.

Recreating tarball
------------------

From a checkout of mraa you can do the following to 'generate' this tarball

~~~~~~~~~~~~~{.sh}
mkdir build
cd build
cmake -DBUILDSWIGNODE=ON  ..
make npmpkg
cd ../
tar czv --exclude='build/*' --exclude='.git/*' -f /tmp/mraa.tar.gz .
~~~~~~~~~~~~~
