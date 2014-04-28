%module(docstring="Python interface to libmaa") pymaa
%feature("autodoc", "1");

%rename("%(strip:[maa_])s") "";

%include ../maa.i
