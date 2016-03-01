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
    #include "java/mraajni.h"
    #include "mraa_lang_func.h"
    extern mraa_lang_func_t* lang_func;

    jint JNI_OnLoad(JavaVM *vm, void *reserved) {
        /* initialize mraa and set jni functions */
        mraa_result_t res = mraa_init();
        if (res == MRAA_SUCCESS || res == MRAA_ERROR_PLATFORM_ALREADY_INITIALISED) {
            mraa_java_set_jvm(vm);
            lang_func->java_isr_callback = &mraa_java_isr_callback;
            lang_func->java_attach_thread  = &mraa_java_attach_thread;
            lang_func->java_detach_thread = &mraa_java_detach_thread;
            lang_func->java_create_global_ref = &mraa_java_create_global_ref;
            lang_func->java_delete_global_ref = &mraa_java_delete_global_ref;
        }
        return JNI_REQUIRED_VERSION;
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
