/*
 * Author: Mihai Stefanescu <mihai.stefanescu@rinftech.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
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
