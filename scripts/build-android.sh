#!/usr/bin/env bash
#
# The script is used to build .aar packages for Mraa modules.
#
# Author: Nicolas Oliver <dario.n.oliver@intel.com>
# Copyright (c) 2017 Intel Corporation.
# SPDX-License-Identifier: MIT
#
# All environment variables used are passed from docker-compose.
# Those environment variables can also be declared in the host,
# and this script can be used to build .aar packages without using
# docker-compose.
#
# Environment variables:
#    - NDK_HOME - Path to Android NDK install folder. i.e. /opt/android-ndk-r14b/
#    - ANDROIDTHINGS_NATIVE_LIB - Path to Android Things library install folder. i.e /opt/native-libandroidthings-0.5.1-devpreview/
#    - JAVA_HOME - Path to Java install folder. i.e. /usr/lib/jvm/java-8-openjdk-amd64/
#
set -e

# Check required environment variables and exit if they are not set
function check_environment {
  VAR_NAME=$1
  VAR_VALUE=$2
  # Check required parameters
  VAR_NAME=${VAR_NAME:?value not provided}
  # Chek if variable is set
  if [ -z "${VAR_VALUE}" ]; then
    echo "Required environment variable ${VAR_NAME} is not defined. Exiting..."
    exit 1;
  else
    echo "Required environment variable ${VAR_NAME} is set."
  fi
}

# Check for required environment variables
check_environment "NDK_HOME" ${NDK_HOME}
check_environment "ANDROIDTHINGS_NATIVE_LIB" ${ANDROIDTHINGS_NATIVE_LIB}
check_environment "JAVA_HOME" ${JAVA_HOME}

PKG_CONFIG_LIBDIR="$NDK_HOME/platforms/android-24/arch-x86/usr/lib"
PKG_CONFIG_SYSROOT_DIR=""
export PKG_CONFIG_LIBDIR

# Run cmake
cmake \
  -DANDROID_COMPILER_FLAGS_CXX='-std=c++11' \
  -DANDROID_PIE=1 \
  -DANDROID_PLATFORM=android-24 \
  -DANDROID_STL_FORCE_FEATURES=on \
  -DANDROID_STL=c++_shared \
  -DANDROID_TOOLCHAIN_NAME=x86-i686 \
  -DCMAKE_INSTALL_PREFIX=install \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=BOTH \
  -DCMAKE_FIND_ROOT_PATH=$NDK_HOME/platforms/android-24/arch-x86/ \
  -DCMAKE_MODULE_PATH=$ANDROIDTHINGS_NATIVE_LIB \
  -DCMAKE_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake \
  -DJAVA_AWT_INCLUDE_PATH=$JAVA_HOME/include \
  -DJAVA_AWT_LIBRARY=$JAVA_HOME/jre/lib/amd64/libjawt.so \
  -DJAVA_INCLUDE_PATH=$JAVA_HOME/include \
  -DJAVA_INCLUDE_PATH2=$JAVA_HOME/include/linux \
  -DJAVA_JVM_LIBRARY=$JAVA_HOME/jre/lib/amd64/server/libjvm.so \
  -DBUILDARCH=$BUILDARCH \
  -DBUILDDOC=$BUILDDOC \
  -DBUILDSWIG=$BUILDSWIG \
  -DBUILDSWIGPYTHON=$BUILDSWIGPYTHON \
  -DBUILDSWIGNODE=$BUILDSWIGNODE \
  -DBUILDSWIGJAVA=$BUILDSWIGJAVA \
  -DUSBPLAT=$USBPLAT \
  -DFIRMATA=$FIRMATA \
  -DONEWIRE=$ONEWIRE \
  -DJSONPLAT=$JSONPLAT \
  -DIMRAA=$IMRAA \
  -DFTDI4222=$FTDI4222 \
  -DENABLEEXAMPLES=$ENABLEEXAMPLES \
  -DINSTALLTOOLS=$INSTALLTOOLS \
  -DBUILDTESTS=$BUILDTESTS \
  -DUSEPYTHON3TESTS=$USEPYTHON3TESTS \
  -H. \
  -Bbuild

make -Cbuild
make -Cbuild install

# Anotate the .java src from doxygen
echo src/mraa.i > build/swig.i.list
# TODO: install doxy port tool
#../doxy/doxyport \
#  build/swig.i.list \
#  -s api,api/mraa \
#  -d build/src/java \
#  --convert-protected-to-private \
#  -o mraa-java-files.txt \
#  -m examples/samples.mapping.txt

# Copy the STL implementation into the build/src directory
cp $NDK_HOME/sources/cxx-stl/llvm-libc++/libs/x86/libc++_shared.so build/src/

# Template for res/values/values.xml in aar
values_xml='<?xml version="1.0" encoding="utf-8"?><resources><string name="app_name">XXX_LIBRARY</string></resources>'

# Template for AndroidManifest.xml in aar
AndroidManifest_xml='<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="XXX_PACKAGE.XXX_LIBRARY"
    android:versionCode="1"
    android:versionName="XXX_VERSION" >
    <uses-sdk android:minSdkVersion="23" android:targetSdkVersion="25"/>
    <application android:allowBackup="true" android:label="@string/app_name" android:supportsRtl="true" />
</manifest>'

# Package prefix
JAVA_PKG_PREFIX_DOT="org.eclipse.mraa.at"

# Package prefix slash to dot
JAVA_PKG_PREFIX_SLASH="${JAVA_PKG_PREFIX_DOT//\./\/}/upm"

# Work in the MRAA build directory
cd build

# Get the MRAA version string from the .pom file
VERSION=`ls src/java/*.pom | sed 's/\([^-]\+\)\-\([0-9]\+\.[0-9]\+\.[0-9]\+\)\.pom/\2/'`

# Get the MRAA name string from the .pom file
LIB_NAME=`ls src/java/*.pom | sed 's/^.*\/\([^-]\+\)\-\([0-9]\+\.[0-9]\+\.[0-9]\+\)\.pom/\1/'`

# Get the MRAA description string from the .pom file
LIB_DESCRIPTION=$(grep -oPm1 "(?<=<description>)[^<]+" src/java/$LIB_NAME-$VERSION.pom)

# Bundle upload directory
PKG_BUNDLE_DIR=BUNDLE_DIR
mkdir -p ${PKG_BUNDLE_DIR}

# Create the aar directory structure
AAR_DIRECTORY="$LIB_NAME-$VERSION"
mkdir -p $AAR_DIRECTORY/jni/x86
mkdir -p $AAR_DIRECTORY/res/values

# Write out res/values/values.xml to the aar
echo "${values_xml/XXX_LIBRARY/$LIB_NAME}" > $AAR_DIRECTORY/res/values/values.xml

# Write out AndroidManifest.xml to the aar
_tmp_manifest=${AndroidManifest_xml/XXX_PACKAGE/$JAVA_PKG_PREFIX_DOT}
_tmp_manifest=${_tmp_manifest/XXX_LIBRARY/$LIB_NAME}
echo "${_tmp_manifest/XXX_VERSION/$VERSION}" > $AAR_DIRECTORY/AndroidManifest.xml

# Copy over the so's
cp src/*.so $AAR_DIRECTORY/jni/x86
cp src/java/*.so $AAR_DIRECTORY/jni/x86

# Create javadoc jar
javadoc src/java/*.java -d $LIB_NAME-javadoc
jar cf $PKG_BUNDLE_DIR/$LIB_NAME-$VERSION-javadoc.jar $LIB_NAME-javadoc/*

# Create sources jar file for upload
jar cf $PKG_BUNDLE_DIR/$LIB_NAME-$VERSION-sources.jar src/java/*.java

# HACK - mraa pom file groupId is io.mraa (AT is io.mraa.at)
perl -p -i.bak -e 's/(groupId>)(.*?)(<\/groupId)/\1org.eclipse.mraa.at\3/' src/java/$LIB_NAME-$VERSION.pom

# HACK - mraa pom file packaging is 'jar' (convert this to 'aar')
perl -p -i.bak -e 's/(packaging>)(.*?)(<\/packaging)/\1aar\3/' src/java/$LIB_NAME-$VERSION.pom

# Copy pom file
cp src/java/$LIB_NAME-$VERSION.pom $PKG_BUNDLE_DIR

# Copy the java module jar to classes.jar
cp src/java/$LIB_NAME.jar $AAR_DIRECTORY/classes.jar

# Create the .aar
pushd $AAR_DIRECTORY
    jar cf ../$PKG_BUNDLE_DIR/$LIB_NAME-$VERSION.aar *
popd
