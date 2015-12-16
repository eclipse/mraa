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

%typemap(jtype) uint8_t * "byte[]"
%typemap(jstype) uint8_t * "byte[]"
%typemap(jni) uint8_t * "jbyteArray"
%typemap(javaout) uint8_t * {
    return $jnicall;
}

%typemap(jtype) jobject runnable "java.lang.Runnable"
%typemap(jstype) jobject runnable "java.lang.Runnable"

namespace mraa {
class Spi;
%typemap(out) uint8_t*
{
  /* need to loop over length */
  $result = JCALL1(NewByteArray, jenv, arg3);
  JCALL4(SetByteArrayRegion, jenv, $result, 0, arg3, (jbyte *) $1);
  free($1);
}
}

%ignore write(const char* data, int length);
%ignore read(char* data, int length);
%ignore globVM;
%ignore env_key;
%ignore mraa_java_isr_callback;

%include ../mraa.i

%wrapper %{
    JavaVM *globVM;

    jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        /* initialize mraa */
        globVM = vm;
        mraa_init();
        return JNI_VERSION_1_8;
    }
%}

%pragma(java) jniclasscode=%{
    static {
        try {
            System.loadLibrary("mraajava");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
%}
