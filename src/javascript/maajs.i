%module (docstring="Javascript interface to libmaa") maajs
%{
    #include "maa.h"
%}

%include <node.i>

%rename("%(strip:[maa_])s") "";

%feature("autodoc", "1");

#ifdef DOXYGEN
%include ../maa_doc.i
%include ../gpio_doc.i
%include ../i2c_doc.i
%include ../pwm_doc.i
#endif

%include ../maa.i
