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

You can run the following commands to 'generate' this tarball from a checkout
of mraa.  It's important to not leave a .git directory as npm will then
consider the tarball contains a full git tree. You also you need a top level
dir which matches the npm pkg name so we create one with a symlink and add the
'h' flag to tar to follow symlinks.

~~~~~~~~~~~~~{.sh}
mkdir build
cd build
cmake -DBUILDSWIGNODE=ON  ..
make npmpkg
ln -s ../ mraa
tar hczv --exclude='build*' --exclude='.gitignore' --exclude='.git' --exclude='build*/*' --exclude='.git/*' -f mraa.tar.gz mraa
~~~~~~~~~~~~~

Building with node-gyp
----------------------

You can build with node-gyp using the binding.gyp file, obviously this requires
either the prebuilt tarball from npm or to do the above to generate it with the
npmpkg target. Note the --target='' flag will not work since binding.gyp will
do a check for the running node v8 version, you'll also need to run that
version (or close to) in your $PATH.

~~~~~~~~~~~{.sh}
node-gyp configure --python=/usr/bin/python2
node-gyp build --python=/usr/bin/python2 --verbose
~~~~~~~~~~~

Note that no cross compilation is available without modifying the --arch flag
manually. If you want to cross compile, it's recommended to use cmake.

