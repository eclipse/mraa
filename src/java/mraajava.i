%module (docstring="Java interface to libmraa") mraa

%feature("autodoc", "3");

%typemap(jtype) (uint8_t *txBuf, int length) "byte[]"
%typemap(jstype) (uint8_t *txBuf, int length) "byte[]"
%typemap(jni) (uint8_t *txBuf, int length) "jbyteArray"
%typemap(javain) (uint8_t *txBuf, int length) "$javainput"

%typemap(in,numinputs=1) (uint8_t *txBuf, int length) {
  $1 = JCALL2(GetByteArrayElements, jenv, $input, NULL);
  $2 = JCALL1(GetArrayLength, jenv, $input);
}

%typemap(jtype) (uint8_t *data, int length) "byte[]"
%typemap(jstype) (uint8_t *data, int length) "byte[]"
%typemap(jni) (uint8_t *data, int length) "jbyteArray"
%typemap(javain) (uint8_t *data, int length) "$javainput"

%typemap(in,numinputs=1) (uint8_t *data, int length) {
  $1 = JCALL2(GetByteArrayElements, jenv, $input, NULL);
  $2 = JCALL1(GetArrayLength, jenv, $input);
}

%typemap(argout) (uint8_t *data, int length) {
  JCALL3(ReleaseByteArrayElements, jenv, $input, $1, JNI_COMMIT);
}



%typemap(jtype) (const uint8_t *data, int length) "byte[]"
%typemap(jstype) (const uint8_t *data, int length) "byte[]"
%typemap(jni) (const uint8_t *data, int length) "jbyteArray"
%typemap(javain) (const uint8_t *data, int length) "$javainput"
%typemap(in) (const uint8_t *data, int length) {
  $1 = JCALL2(GetByteArrayElements, jenv, $input, NULL);
  $2 = JCALL1(GetArrayLength, jenv, $input);
}


/*
%include cpointer.i*/

/*%include "arrays_java.i"
%newobject I2c::read(char *data, int length);
%newobject Spi::write(char *data, int length);

namespace mraa {
class Spi;
}


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

