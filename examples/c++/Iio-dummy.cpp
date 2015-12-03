/*
 * Author: Henry Bruce
 * Copyright (c) 2015 Intel Corporation.
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

// #include <unistd.h>
#include <iostream> 
#include <math.h>
#include <float.h>
#include "mraa/iio.hpp"

#define EXPECT_FAILURE 0
#define EXPECT_SUCCESS 1

#define IIO_RUN(func, attr, value, expect) \
{ \
    std::string attr_name = attr; \
    bool success = true;    \
    try { \
        iio_device->func(attr_name, value);  \
    } catch (std::exception& e) { \
        success = false; \
    } \
    log_result(#func, attr_name, expect, success); \
}

#define IIO_TEST(func, attr, value, expect) \
{ \
    std::string attr_name = attr; \
    bool success = false;    \
    try { \
        success = fabs(iio_device->func(attr_name) - value) < FLT_EPSILON; \
    } catch (std::exception& e) { \
        success = false; \
    } \
    log_result(#func, attr_name, expect, success); \
}

mraa::Iio* iio_device;

void log_result(std::string test_name, std::string attr_name, bool expect_success, bool success)
{
    std::string result;
    if (expect_success)
       result = success ? "PASS" : "FAIL";
    else
       result = success ? "FAIL" : "PASS";
    fprintf(stdout, "%s(%s): %s\n", test_name.c_str(), attr_name.c_str(), result.c_str());
}


int
main()
{
    try {
        iio_device = new mraa::Iio(0);      
    } catch (std::exception& e) {
        std::cerr << "IIO device 0 not found" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        mraa::Iio* bad_iio_device = new mraa::Iio(1);        
        delete bad_iio_device;
    } catch (std::exception& e) {
        std::cerr << "IIO device 1 not found" << std::endl;
    }

    std::cout << "Using IIO device0. Name is " << iio_device->getDeviceName() << std::endl;          
    IIO_RUN(writeFloat, "in_accel_x_raw", 100, EXPECT_FAILURE);
    IIO_RUN(writeFloat, "in_voltage0_scale", 100, EXPECT_FAILURE);    
    IIO_RUN(writeInt, "out_voltage0_raw", 100, EXPECT_SUCCESS);        
    IIO_TEST(readInt, "in_accel_x_raw", 34, EXPECT_SUCCESS);
    IIO_TEST(readFloat, "in_voltage0_scale", 0.001333, EXPECT_SUCCESS);    
    delete iio_device;

    return EXIT_SUCCESS;
}

