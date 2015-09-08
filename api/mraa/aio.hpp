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

#include <stdexcept>
#include "aio.h"
#include "types.hpp"

namespace mraa
{

/**
 * @brief API to Analog IO
 *
 * This file defines the aio interface for libmraa
 *
 * @snippet examples/c++/AioA0.cpp Interesting
 */
class Aio
{
  public:
    /**
     * Aio Constructor, takes a pin number which will map directly to the
     * board number
     *
     * @param pin channel number to read ADC inputs
     */
    Aio(unsigned int pin)
    {
        m_aio = mraa_aio_init(pin);
        if (m_aio == NULL) {
            throw std::invalid_argument("Invalid AIO pin specified - do you have an ADC?");
        }
    }
    /**
     * Aio destructor
     */
    ~Aio()
    {
        mraa_aio_close(m_aio);
    }
    /**
     * Read a value from the AIO pin. By default mraa will shift
     * the raw value up or down to a 10 bit value.
     *
     * @returns The current input voltage. By default, a 10bit value
     */
    int
    read()
    {
        return mraa_aio_read(m_aio);
    }
    /**
     * Read a value from the AIO pin and return it as a normalized float.
     *
     * @returns The current input voltage as a normalized float (0.0f-1.0f)
     */
    float
    readFloat()
    {
        return mraa_aio_read_float(m_aio);
    }
    /**
     * Set the bit value which mraa will shift the raw reading
     * from the ADC to. I.e. 10bits
     * @param bits the bits the return from read should be i.e 10
     * @return mraa::Result type
     */
    Result
    setBit(int bits)
    {
        return (Result) mraa_aio_set_bit(m_aio, bits);
    }
    /**
     * Gets the bit value mraa is shifting the analog read to.
     *
     * @return bit value mraa is set return from the read function
     */
    int
    getBit()
    {
        return mraa_aio_get_bit(m_aio);
    }

  private:
    mraa_aio_context m_aio;
};
}
