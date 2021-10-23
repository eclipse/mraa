/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2014-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include "mraa/types.h"

// location for defining JNI version to use
#define JNI_REQUIRED_VERSION JNI_VERSION_1_6

void mraa_java_set_jvm(JavaVM* vm);
void mraa_java_isr_callback(void* args);
mraa_result_t mraa_java_attach_thread();
void mraa_java_detach_thread();
void* mraa_java_create_global_ref(void* args);
void mraa_java_delete_global_ref(void* ref);

#ifdef __cplusplus
}
#endif
