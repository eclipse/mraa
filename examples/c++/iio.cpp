/*
 * Author: Henry Bruce
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Sets accelerometer scale and registers for the threshold event
 *
 */

/* standard headers */
#include <float.h>
#include <iostream>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/iio.hpp"

#define EXPECT_FAILURE 0
#define EXPECT_SUCCESS 1

#define IIO_TRY(func)                         \
    {                                         \
        bool success = true;                  \
        try {                                 \
            iio_device->func;                 \
        } catch (std::exception & e) {        \
            success = false;                  \
        }                                     \
        log_result(#func, "", true, success); \
    }

// Macro to run IIO method on attribute and log output
#define IIO_RUN(func, attr, value, expect)             \
    {                                                  \
        std::string attr_name = attr;                  \
        bool success = true;                           \
        try {                                          \
            iio_device->func(attr_name, value);        \
        } catch (std::exception & e) {                 \
            success = false;                           \
        }                                              \
        log_result(#func, attr_name, expect, success); \
    }

// Macro to run IIO method on attribute and check for expected result and log output
#define IIO_TEST(func, attr, value, expect)                                    \
    {                                                                          \
        std::string attr_name = attr;                                          \
        bool success = false;                                                  \
        try {                                                                  \
            success = fabs(iio_device->func(attr_name) - value) < FLT_EPSILON; \
        } catch (std::exception & e) {                                         \
            success = false;                                                   \
        }                                                                      \
        log_result(#func, attr_name, expect, success);                         \
    }

mraa::Iio* iio_device;
int eventCount = 0;

// Log result of test. Note a "fail" (i.e. success is false) will be displayed as a pass if a fail
// was expected
void
log_result(std::string test_name, std::string attr_name, bool expect_success, bool success)
{
    std::string result;
    if (expect_success)
        result = success ? "PASS" : "FAIL";
    else
        result = success ? "FAIL" : "PASS";
    if (attr_name.empty())
        std::cout << "%s: %s" << test_name.c_str() << result.c_str() << std::endl;
    else
        std::cout << "%s(%s): %s" << test_name.c_str() << attr_name.c_str() << result.c_str() << std::endl;
}

// Generate iio_dummy driver event by writing a string to a specific sysfs node
bool
generate_event()
{
    FILE* fp = fopen("/sys/bus/iio/devices/iio_evgen/poke_ev0", "w");
    if (fp == NULL)
        return false;
    fprintf(fp, "1\n");
    fclose(fp);
    return true;
}


// IIO event handler that checks for event from dummy_iio_evgen driver
class IioTestHandler : public mraa::IioHandler
{
  protected:
    void
    onIioEvent(const mraa::IioEventData& eventData)
    {
        if (eventData.channelType == IIO_VOLTAGE && eventData.direction == IIO_EV_DIR_RISING &&
            eventData.type == IIO_EV_TYPE_THRESH)
            eventCount++;
    }
};

int
main(void)
{
    //! [Interesting]
    IioTestHandler testHandler;
    std::string deviceName;
    try {
        mraa::Iio* iio_device0 = new mraa::Iio(0);
        std::cout << "IIO device 0 found by id." << std::endl;
        deviceName = iio_device0->getDeviceName();
        delete iio_device0;
    } catch (std::exception& e) {
        std::cerr << "IIO device 0 not found." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        mraa::Iio* iio_device1 = new mraa::Iio(1);
        delete iio_device1;
    } catch (std::exception& e) {
        std::cerr << "IIO device 1 not found. This is expected behavior." << std::endl;
    }

    try {
        iio_device = new mraa::Iio(deviceName);
        std::cout << "IIO device 0 found by name." << std::endl;
    } catch (std::exception& e) {
        std::cerr << "IIO device 0 not found." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Using IIO device0. Name is " << iio_device->getDeviceName() << std::endl;
    IIO_RUN(writeFloat, "in_accel_x_raw", 100, EXPECT_FAILURE);
    IIO_RUN(writeFloat, "in_voltage0_scale", 100, EXPECT_FAILURE);
    IIO_RUN(writeInt, "out_voltage0_raw", 100, EXPECT_SUCCESS);
    IIO_TEST(readInt, "in_accel_x_raw", 34, EXPECT_SUCCESS);
    IIO_TEST(readFloat, "in_voltage0_scale", 0.001333, EXPECT_SUCCESS);
    IIO_RUN(writeInt, "events/in_voltage0_thresh_rising_en", 1, EXPECT_SUCCESS);
    IIO_TRY(registerEventHandler(&testHandler));
    eventCount = 0;
    generate_event();
    usleep(500000);
    log_result("eventReceived", "", (eventCount == 1), true);
    //! [Interesting]

    delete iio_device;

    return EXIT_SUCCESS;
}
