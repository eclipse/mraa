This directory used to contain an OE-Toolchain file. A far better one is now included
with Devtool/OE-SDK.

https://git.yoctoproject.org/cgit.cgi/poky/plain/meta/recipes-devtools/cmake/cmake/OEToolchainConfig.cmake

The following may be interesting for certain Java/Cross-compilation reasons:
```
# for Cmake java
set( JAVA_AWT_INCLUDE_PATH $ENV{JAVA_HOME}/include CACHE PATH "" FORCE)
set( JAVA_AWT_LIBRARY $ENV{JAVA_HOME}/jre/lib/amd64/libjawt.so CACHE FILEPATH "" FORCE)
set( JAVA_INCLUDE_PATH $ENV{JAVA_HOME}/include CACHE PATH "" FORCE)
set( JAVA_INCLUDE_PATH2 $ENV{JAVA_HOME}/include/linux CACHE PATH "" FORCE)
set( JAVA_JVM_LIBRARY $ENV{JAVA_HOME}/jre/lib/amd64/libjvm.so CACHE FILEPATH "" FORCE)
```