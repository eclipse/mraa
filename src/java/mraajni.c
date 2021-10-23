/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <pthread.h>
#include "java/mraajni.h"

static pthread_key_t env_key;
static pthread_once_t env_key_init = PTHREAD_ONCE_INIT;
static jmethodID runGlobal;
static JavaVM* globVM = NULL;
static jclass jcObject;

void
mraa_java_set_jvm(JavaVM* vm)
{
    globVM = vm;
}

static void
mraa_java_make_env_key(void)
{
    if (globVM != NULL) {
        JNIEnv* jenv;
        (*globVM)->GetEnv(globVM, (void**) &jenv, JNI_REQUIRED_VERSION);
        jclass rcls = (*jenv)->FindClass(jenv, "java/lang/Runnable");
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionClear(jenv);
            return;
        }

        jcObject = (jclass) (*jenv)->NewGlobalRef(jenv, rcls);
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionClear(jenv);
            return;
        }
        (*jenv)->DeleteLocalRef(jenv, rcls);

        runGlobal = (*jenv)->GetMethodID(jenv, jcObject, "run", "()V");
        if ((*jenv)->ExceptionOccurred(jenv)) {
            (*jenv)->ExceptionClear(jenv);
            return;
        }

        pthread_key_create(&env_key, NULL);
    }
}

void
mraa_java_isr_callback(void* data)
{
    JNIEnv* jenv = (JNIEnv*) pthread_getspecific(env_key);
    (*jenv)->CallVoidMethod(jenv, (jobject) data, runGlobal);
}

mraa_result_t
mraa_java_attach_thread()
{
    if (globVM != NULL) {
        JNIEnv* jenv;
        jint err = (*globVM)->AttachCurrentThreadAsDaemon(globVM, (void**) &jenv, NULL);
        if (err == JNI_OK) {
            pthread_once(&env_key_init, mraa_java_make_env_key);
            pthread_setspecific(env_key, jenv);
            return MRAA_SUCCESS;
        }
    }
    return MRAA_ERROR_UNSPECIFIED;
}

void
mraa_java_detach_thread()
{
    if (globVM != NULL)
        (*globVM)->DetachCurrentThread(globVM);
}

void*
mraa_java_create_global_ref(void* args)
{
    if (globVM != NULL) {
        JNIEnv* jenv;
        (*globVM)->GetEnv(globVM, (void**) &jenv, JNI_REQUIRED_VERSION);
        jobject grunnable = (*jenv)->NewGlobalRef(jenv, (jobject) args);
        return (void*) grunnable;
    } else
        return NULL;
}

void
mraa_java_delete_global_ref(void* ref)
{
    if (globVM != NULL) {
        JNIEnv* jenv;
        (*globVM)->GetEnv(globVM, (void**) &jenv, JNI_REQUIRED_VERSION);
        (*jenv)->DeleteGlobalRef(jenv, (jobject) ref);
    }
}
