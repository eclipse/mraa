/*
 * Author: Noel Eck <noel.eck@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */


#include "gtest/gtest.h"
#include "mraa/common.h"
#include "include/mraa_internal_types.h"

/* MRAA API common test fixture */
class api_common_h_unit : public ::testing::Test
{
    protected:
        /* One-time setup logic if needed */
        api_common_h_unit() {}

        /* One-time tear-down logic if needed */
        virtual ~api_common_h_unit() {}

        /* Per-test setup logic if needed */
        virtual void SetUp() {}

        /* Per-test tear-down logic if needed */
        virtual void TearDown() {}
};

/* Test for a successful mraa_init */
TEST_F(api_common_h_unit, test_libmraa_init_doa)
{
    ASSERT_EQ(MRAA_SUCCESS,  mraa_init());
}

/* Test for a basic mraa init/deinit */
TEST_F(api_common_h_unit, test_libmraa_init_deinit)
{
    ASSERT_EQ(MRAA_SUCCESS,  mraa_init());
    mraa_deinit();
}

/* Test the C exposed common methods */
TEST_F(api_common_h_unit, test_libmraa_common_methods)
{
    ASSERT_EQ(MRAA_SUCCESS,  mraa_init());

    /* Test log level settings */
    for (int lvl = 0; lvl <= 7;lvl++)
        ASSERT_EQ(MRAA_SUCCESS, mraa_set_log_level(lvl)) << "Set loglevel: " << lvl;
    ASSERT_EQ(MRAA_ERROR_INVALID_PARAMETER, mraa_set_log_level(8));

    /* Mock platform tests */
    if (mraa_get_platform_type() == MRAA_MOCK_PLATFORM)
    {
        /* Pin 0 is a valid pin */
        ASSERT_TRUE(mraa_pin_mode_test(0, MRAA_PIN_VALID));

        /* Pin 0 is a GPIO */
        ASSERT_TRUE(mraa_pin_mode_test(0, MRAA_PIN_GPIO));

        /* Mock platform defaults to 12 bits */
        ASSERT_EQ(12, mraa_adc_raw_bits());

        /* Which better equal the bitsize for platform 0 */
        ASSERT_EQ(mraa_get_platform_adc_raw_bits(0), mraa_adc_raw_bits());

        /* Raw bits get shifted to supported bits */
        ASSERT_EQ(10, mraa_adc_supported_bits());

        /* Check the mock platform name */
        ASSERT_STREQ("MRAA mock platform", mraa_get_platform_name());

        /* No versioning for the mock platform */
        ASSERT_STREQ(NULL, mraa_get_platform_version(0));

        /* Currently 10 pins in the mock platform */
        ASSERT_EQ(10, mraa_get_pin_count());

        /* Test the other pin/bus counts for the mock platform */
        EXPECT_EQ(1, mraa_get_aio_count());
        EXPECT_EQ(1, mraa_get_gpio_count());
        EXPECT_EQ(1, mraa_get_i2c_bus_count());
        EXPECT_EQ(0, mraa_get_i2c_bus_id(0));
        EXPECT_EQ(0, mraa_get_pwm_count());
        EXPECT_EQ(1, mraa_get_spi_bus_count());
        EXPECT_EQ(1, mraa_get_uart_count());

        /* Test pin to name method/s */
        ASSERT_STREQ("GPIO0", mraa_get_pin_name(0));

        /* Test the lookup method/s */
        ASSERT_EQ(0, mraa_gpio_lookup("GPIO0"));

        /* MOCK does NOT have a subplatform */
        ASSERT_FALSE(mraa_has_sub_platform());
    }

    /* Set the priority of this process */
    //EXPECT_EQ(40, mraa_set_priority(40));
}
