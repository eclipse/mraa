/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
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

/** @file
 *
 * This file defines the spi C++ interface for libmaa
 */

#include "spi.h"

namespace maa {

class Spi {
    public:
        Spi(int bus) {
            m_spi = maa_spi_init(bus);
        }
        ~Spi() {
            maa_spi_stop(m_spi);
        }
        maa_result_t mode(unsigned short mode) {
            return maa_spi_mode(m_spi, mode);
        }
        maa_result_t frequency(int hz) {
            return maa_spi_frequency(m_spi, hz);
        }
        unsigned char write(uint8_t data) {
            return (unsigned char) maa_spi_write(m_spi, data);
        }
        unsigned char* write_buf(uint8_t* data, int length) {
            return (unsigned char*) maa_spi_write_buf(m_spi, data, length);
        }
    private:
        maa_spi_context m_spi;
};

}
