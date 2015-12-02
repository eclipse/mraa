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

#include <unistd.h>
#include <math.h>
#include <float.h>
#include "mraa/iio.h"

#define EXPECT_FAILURE 0
#define EXPECT_SUCCESS 1

void log_result(const char* test_name, const char* attr_name, mraa_boolean_t expect_success, mraa_result_t test_result)
{
    char* result;
    if (expect_success)
       result = test_result == MRAA_SUCCESS ? "PASS" : "FAIL";
    else
       result = test_result == MRAA_SUCCESS ? "FAIL" : "PASS";
    fprintf(stdout, "%s(%s): %s\n", test_name, attr_name, result);
}

int
main()
{
    mraa_iio_context iio_device = mraa_iio_init(0);
    char* attr_name;
    float iio_float;
    int iio_integer;
    mraa_result_t ret;

    if (iio_device == NULL) {
        fprintf(stderr, "IIO device %d not found\n", 0);
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Using IIO device %s\n", mraa_iio_get_device_name(iio_device));

    attr_name = "in_accel_x_raw";
    ret = mraa_iio_write_float(iio_device, attr_name, 100);
    log_result("iio_write_float", attr_name, EXPECT_FAILURE, ret);

    attr_name = "in_voltage0_scale";
    ret = mraa_iio_write_float(iio_device, attr_name, 100);
    log_result("iio_write_float", attr_name, EXPECT_FAILURE, ret);

    attr_name = "out_voltage0_raw";
    ret = mraa_iio_write_integer(iio_device, attr_name, 100);
    log_result("iio_write_integer", attr_name, EXPECT_SUCCESS, ret);

    attr_name = "in_accel_x_raw";
    ret = mraa_iio_read_integer(iio_device, attr_name, &iio_integer);
    ret = iio_integer == 34 ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
    log_result("iio_read_integer", attr_name, EXPECT_SUCCESS, ret);

    attr_name = "in_voltage0_scale";
    ret = mraa_iio_read_float(iio_device, attr_name, &iio_float);
    ret = fabs(iio_float - 0.001333) < FLT_EPSILON ? MRAA_SUCCESS : MRAA_ERROR_UNSPECIFIED;
    log_result("iio_read_float", attr_name, EXPECT_SUCCESS, ret);

    attr_name = "";
    ret = mraa_iio_stop(iio_device);
    log_result("iio_stop", attr_name, EXPECT_SUCCESS, ret);

    return EXIT_SUCCESS;
}
