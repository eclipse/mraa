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

#include "aio.h"

namespace mraa {

/**
 * @brief C++ API to Analog IO
 *
 * This file defines the aio C++ interface for libmraa
 *
 * @snippet examples/c++/AioA0.cpp Interesting
 */
class Aio {
    public:
        /**
         * Aio Constructor, takes a pin number which will map directly to the
         * board number
         *
         * @param pin channel number to read ADC inputs
         */
        Aio(unsigned int pin) {
            m_aio = mraa_aio_init(pin);
        }
        /**
         * Aio destructor
         */
        ~Aio() {
            mraa_aio_close(m_aio);
        }
        /**
         * Read a value from the AIO pin. Note this value can never be outside
         * of the bounds of an unsigned short
         *
         * @returns The current input voltage, normalised to a 16-bit value
         */
        int read() {
            // Use basic types to make swig code generation simpler
            return (int) mraa_aio_read(m_aio);
        }
    private:
        mraa_aio_context m_aio;
};

}
