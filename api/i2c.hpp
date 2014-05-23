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
 * This file defines the i2c C++ interface for libmaa
 *
 */

#include "i2c.h"

namespace maa {

class I2c {
    public:
        I2c(int bus, bool raw=false) {
            if (raw)
                m_i2c = maa_i2c_init_raw(bus);
            else
                m_i2c = maa_i2c_init(bus);
        }
        ~I2c() {
            maa_i2c_stop(m_i2c);
        }
        maa_result_t frequency(int hz) {
            return maa_i2c_frequency(m_i2c, hz);
        }
        maa_result_t address(int address) {
            return maa_i2c_address(m_i2c, address);
        }
        uint8_t read_byte() {
            return maa_i2c_read_byte(m_i2c);
        }
        maa_result_t read(uint8_t* data, int length) {
            return maa_i2c_read(m_i2c, data, length);
        }
        maa_result_t write(const uint8_t* data, int length) {
            return maa_i2c_write(m_i2c, data, length);
        }
        maa_result_t write_byte(const uint8_t data) {
            return maa_i2c_write_byte(m_i2c, data);
        }
    private:
        maa_i2c_context m_i2c;
};

}
