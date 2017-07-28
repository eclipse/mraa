%module(docstring="Python interface to libmraa") mraa                                                                                          

%feature("autodoc", "3");

#ifdef DOXYGEN
  %include common_hpp_doc.i
  %include gpio_class_doc.i
  %include i2c_class_doc.i
  %include pwm_class_doc.i
  %include aio_class_doc.i
  %include spi_class_doc.i
  %include uart_class_doc.i
#endif

%include ../mraapython.i
