/*
 * Author: Mihai Stefanescu <mihai.stefanescu@rinftech.com>
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

#include "mraa/initio.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <exception>


/* MRAA IO INIT hpp test fixture */
class mraa_initio_hpp_unit : public ::testing::Test
{
};

/* Test for an invalid AIO init. */
TEST_F(mraa_initio_hpp_unit, test_aio_init_invalid)
{
    ASSERT_THROW(mraa::MraaIo io("a:bogus:10"), std::runtime_error);
}

/* Test for a valid AIO init. */
TEST_F(mraa_initio_hpp_unit, test_aio_init_valid)
{
    mraa::MraaIo io("a:0:10");
    ASSERT_EQ(1, io.aios.size());
}
