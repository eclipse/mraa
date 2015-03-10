%module (docstring="Javascript interface to libmraa") mraa

%feature("autodoc", "3");
#define NODE012

%include carrays.i
%include cpointer.i
%array_class(uint8_t, uint8Array);

%inline %{
  #include <node_buffer.h>
%}

%typemap(in) (const uint8_t *data, int length) {
  $1 = (uint8_t*) node::Buffer::Data($input);
  $2 = node::Buffer::Length($input);
}

%typemap(in) (uint8_t *txBuf, int length) {
  $1 = (uint8_t*) node::Buffer::Data($input);
  $2 = node::Buffer::Length($input);
}

%typemap(in) (v8::Handle<v8::Function> func) {
  $1 = v8::Local<v8::Function>::Cast($input);
}

namespace mraa {
class Spi;
%typemap(out) uint8_t*
{
#ifdef NODE012
  $result = node::Buffer::New((char*) $1, arg3);
#else
  $result = node::Buffer::New((char*) $1, arg3)->handle_;
#endif
}
}

%newobject I2c::read(uint8_t *data, int length);
%newobject Spi::write(uint8_t *data, int length);

%typemap(in) (uint8_t *data, int length) {
   int x;
   int ecode = SWIG_AsVal_int($input, &x);
   if (!SWIG_IsOK(ecode)) {
     SWIG_exception_fail(SWIG_ArgError(ecode), "Expected an int");
   }
   $2 = x;
   if ($2 < 0) {
       SWIG_exception_fail(SWIG_ERROR, "Positive integer expected");
       SWIGV8_RETURN(SWIGV8_UNDEFINED());
   }
   $1 = (uint8_t*) malloc($2 * sizeof(uint8_t));
}

%typemap(argout) (uint8_t *data, int length) {
   if (result < 0) {      /* Check for I/O error */
       free($1);
       SWIG_exception_fail(SWIG_ERROR, "I2c write failed");
       SWIGV8_RETURN(SWIGV8_UNDEFINED());
   }
#ifdef NODE012
   $result = node::Buffer::New((char*) $1, result);
#else
   $result = node::Buffer::New((char*) $1, result)->handle_;
#endif
   free($1);
}

%include ../mraa.i
