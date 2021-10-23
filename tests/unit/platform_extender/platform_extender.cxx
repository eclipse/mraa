/*
 * Author: Noel Eck <noel.eck@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */


#include <dlfcn.h>

#include "gtest/gtest.h"
#include "mraa/common.h"
#include "mraa_internal_types.h"

/* MRAA test fixture */
class plaform_extender : public ::testing::Test {};

/* Not much to test w/o hw - test a no-init case.
 * Note, w/o a link to libmraa, this binary does NOT initialize mraa
 * with the loader constructor attribute.  This is intended. */
TEST_F(plaform_extender, test_no_init)
{
    mraa_board_t plat = {};

    void* usblib = dlopen("libmraa-platform-ft4222.so", RTLD_LAZY);
    ASSERT_NE((void*)NULL, usblib)
        << "Failed to load libmraa-platform-ft4222.so, reason (" << dlerror()
        << ")";

    fptr_add_platform_extender add_ft4222_platform =
        (fptr_add_platform_extender)dlsym(usblib, "mraa_usb_platform_extender");

    ASSERT_NE((void*)NULL, add_ft4222_platform) << "Symbol 'add_ft4222_platform' "
        << "does not exist in libmraa-platform-ft4222.so";

    ASSERT_EQ(MRAA_ERROR_PLATFORM_NOT_INITIALISED, add_ft4222_platform(&plat))
        << "Initialization returned a valid platform.  Make sure no Ft4222 "
        << "device is connected";
}
