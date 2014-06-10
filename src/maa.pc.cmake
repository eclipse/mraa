prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib@LIB_SUFFIX@
includedir=${prefix}/include

Name: maa
Description: Low Level Skeleton Library for Communication
Version: @maa_VERSION_STRING@

Libs: -L${libdir} -lmaa
Cflags: -I${includedir}
