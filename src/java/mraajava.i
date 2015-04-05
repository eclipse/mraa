%module (docstring="Java interface to libmraa") mraa

%feature("autodoc", "3");

%include carrays.i
%include cpointer.i
/*
namespace mraa {
class Spi;
}

%newobject I2c::read(char *data, int length);
%newobject Spi::write(char *data, int length);

%typemap(in) (char *data, int length) {
   int x;
   int ecode = SWIG_AsVal_int($input, &x);
   if (!SWIG_IsOK(ecode)) {
     %exception(SWIG_ArgError(ecode), "Expected an int");
   }
   $2 = x;
   if ($2 < 0) {
       SWIG_exception_fail((-1), "Positive integer expected");
       SWIGV8_RETURN(v8::Undefined());
   }
   $1 = (uint8_t*) malloc($2 * sizeof(uint8_t));
}

%typemap(argout) (uint8_t *data, int length) {
   if (result < 0) {      
       free($1);
       SWIG_exception_fail((-1), "I2c write failed");
       SWIGV8_RETURN(v8::Undefined());
   }
   $result = New((char*) $1, result)->handle_;
   free($1);
}
*/

%include ../mraa.i
