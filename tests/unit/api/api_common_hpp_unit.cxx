/*
 * Author: Noel Eck <noel.eck@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */


#include "gtest/gtest.h"
#include "mraa.hpp"

/* MRAA API common test fixture */
class api_common_hpp_unit : public ::testing::Test
{
    protected:
        /* One-time setup logic if needed */
        api_common_hpp_unit() {}

        /* One-time tear-down logic if needed */
        virtual ~api_common_hpp_unit() {}

        /* Per-test setup logic if needed */
        virtual void SetUp() {}

        /* Per-test tear-down logic if needed */
        virtual void TearDown() {}
};

/* Test the C++ exposed common methods */
TEST_F(api_common_hpp_unit, test_libmraa_common_methods)
{
    ASSERT_EQ(mraa::SUCCESS, mraa::init());

    /* Test log level settings */
    for (int lvl = 0; lvl <= 7;lvl++)
        ASSERT_EQ(mraa::SUCCESS, mraa::setLogLevel(lvl)) << "Set loglevel: " << lvl;
    ASSERT_EQ(mraa::ERROR_INVALID_PARAMETER, mraa::setLogLevel(8));

    /* Mock platform tests */
    if (mraa::getPlatformType() == mraa::MOCK_PLATFORM)
    {
        /* Pin 0 is a valid pin */
        ASSERT_TRUE(mraa::pinModeTest(0, mraa::PIN_VALID));

        /* Pin 0 is a GPIO */
        ASSERT_TRUE(mraa::pinModeTest(0, mraa::PIN_GPIO));

        /* Mock platform defaults to 12 bits */
        ASSERT_EQ(12, mraa::adcRawBits());

        /* Raw bits get shifted to supported bits */
        ASSERT_EQ(10, mraa::adcSupportedBits());

        /* Check the mock platform name */
        ASSERT_EQ("MRAA mock platform", mraa::getPlatformName());

        /* No versioning for the mock platform */
        ASSERT_EQ("", mraa::getPlatformVersion(0));

        /* Currently 10 pins in the mock platform */
        ASSERT_EQ(10, mraa::getPinCount());

        /* Test the other pin/bus counts for the mock platform */
        /* Missing equivalent C++ methods */
        //EXPECT_EQ(1, mraa_get_aio_count());
        //EXPECT_EQ(1, mraa_get_gpio_count());
        EXPECT_EQ(1, mraa::getI2cBusCount());
        EXPECT_EQ(0, mraa::getI2cBusId(0));
        //EXPECT_EQ(0, mraa_get_pwm_count());
        //EXPECT_EQ(1, mraa_get_spi_bus_count());
        EXPECT_EQ(1, mraa::getUartCount());

        /* Test pin to name method/s */
        ASSERT_EQ("GPIO0", mraa::getPinName(0));

        /* Test the lookup method/s */
        ASSERT_EQ(0, mraa::getGpioLookup("GPIO0"));

        /* MOCK does NOT have a subplatform */
        ASSERT_FALSE(mraa::hasSubPlatform());
    }
}
