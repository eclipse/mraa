%module(docstring="Lua interface to libmraa") mraa

%inline %{
  #include <inttypes.h>
%}

%include typemaps.i
%include carrays.i

%array_class(uint8_t, uint8Array);

%include ../mraa.i

%init %{
    //Adding mraa_init() to the module initialisation process
    if (mraa_init() != MRAA_SUCCESS)
      fprintf(stderr, "Lua Runtime Error: mraa_init() failed.\n");
%}
