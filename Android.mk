# Copyright (C) 2015 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmraa
LOCAL_CFLAGS += -Wno-unused-parameter -DX86PLAT=1
LOCAL_SHARED_LIBRARIES := libcutils libutils libdl libc
LOCAL_SRC_FILES := \
    src/mraa.c \
    src/gpio/gpio.c \
    src/i2c/i2c.c \
    src/pwm/pwm.c \
    src/spi/spi.c \
    src/aio/aio.c \
    src/uart/uart.c \
    src/x86/x86.c \
    src/iio/iio.c \
    src/x86/intel_galileo_rev_d.c \
    src/x86/intel_galileo_rev_g.c \
    src/x86/intel_edison_fab_c.c  \
    src/x86/intel_de3815.c \
    src/x86/intel_nuc5.c \
    src/x86/intel_sofia_3gr.c \
    src/x86/intel_minnow_byt_compatible.c

# glob.c pulled in from NetBSD project (BSD 3-clause License)
LOCAL_SRC_FILES += \
    src/glob/glob.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/include/linux \
    $(LOCAL_PATH)/include/x86 \
    $(LOCAL_PATH)/api \
    $(LOCAL_PATH)/api/mraa \
    $(LOCAL_PATH)/src/glob

LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH)/api \
    $(LOCAL_PATH)/api/mraa

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

