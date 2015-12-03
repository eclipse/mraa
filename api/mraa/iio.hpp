/*
 * Author: Henry Bruce <henry.bruce@intel.com>
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

#pragma once

#include <stdexcept>
 #include <sstream>
#include "iio.h"
#include "types.hpp"

namespace mraa
{

/**
 * @brief API to Industrial IO
 *
 * This file defines the C++ iio interface for libmraa
 *
 * @snippet iio_dummy_test.cpp Interesting
 */
class Iio
{
  public:
    /**
     * Iio Constructor, takes a device number which will map directly to sysfs
     * e.g. device 0 mAps to /sys/bus/iio/devices/iio:device0
     *
     * @param device IIO device number
     *
     * @throws std::invalid_argument if initialization fails
     */
    Iio(int device)
    {
        m_iio = mraa_iio_init(device);
        if (m_iio == NULL) {
            std::ostringstream oss;
            oss << "IIO device " << device << " is not valid";
            throw std::invalid_argument(oss.str());
        }
    }

    /**
     * Iio destructor
     */
    ~Iio()
    {
        mraa_iio_stop(m_iio);
    }

    
    /**
     * Get device name 
     *
     * @returns The device name
     */
    std::string
    getDeviceName()
    {
        return mraa_iio_get_device_name(m_iio);
    }

    /**
     * Read an integer value from specified attribute.
     *
     * @returns The integer value
     *
     * @throws std::invalid_argument if read fails
     */
    int
    readInt(const std::string& attributeName)
    {
        int value;
        mraa_result_t res = mraa_iio_read_integer(m_iio, attributeName.c_str(), &value);
        if (res != MRAA_SUCCESS) {
            std::ostringstream oss;
            oss << "IIO readInt for attibute " << attributeName << " failed";
            throw std::runtime_error(oss.str());
        }
        return value;
    }

    /**
     * Read a float value from specified attribute. 
     *
     * @returns The float value
     *
     * @throws std::invalid_argument if read fails
     */
    float
    readFloat(const std::string& attributeName)
    {
        float value;
        mraa_result_t res = mraa_iio_read_float(m_iio, attributeName.c_str(), &value);
        if (res != MRAA_SUCCESS) {
            std::ostringstream oss;
            oss << "IIO readFloat for attibute " << attributeName << " failed";
            throw std::runtime_error(oss.str());
        }
        return value;
    }

    /**
     * Write an integer value to specified attribute. 
     *
     * @throws std::invalid_argument if write fails
     */
    void
    writeInt(const std::string& attributeName, int value)
    {
        mraa_result_t res = mraa_iio_write_integer(m_iio, attributeName.c_str(), value);
        if (res != MRAA_SUCCESS) {
            std::ostringstream oss;
            oss << "IIO writeInt for attibute " << attributeName << " failed";
            throw std::runtime_error(oss.str());
        }

    }

    /**
     * Write a float value to specified attribute. 
     *
     * @throws std::invalid_argument if write fails
     */
    void
    writeFloat(const std::string& attributeName, float value)
    {
        mraa_result_t res = mraa_iio_write_float(m_iio, attributeName.c_str(), value);
        if (res != MRAA_SUCCESS) {
            std::ostringstream oss;
            oss << "IIO writeFloat for attibute " << attributeName << " failed";
            throw std::runtime_error(oss.str());
        }

    }


  private:
    mraa_iio_context m_iio;
};
}
