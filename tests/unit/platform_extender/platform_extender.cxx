/*
 * Author: Noel Eck <noel.eck@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
