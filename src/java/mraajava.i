%module (directors="1",docstring="Java interface to libmraa") mraa

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

%feature("director") IsrCallback;
%include ../mraa.i
