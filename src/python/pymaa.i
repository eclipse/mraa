%module(docstring="Python interface to libmaa") pymaa

%feature("autodoc", "3");

#ifdef DOXYGEN
%include ../maa_doc.i
%include ../gpio_doc.i
%include ../i2c_doc.i
%include ../pwm_doc.i
#endif

%include ../maa.i

