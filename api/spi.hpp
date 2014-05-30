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

#include "spi.h"

namespace maa {

/**
 * @brief C++ API to System Packet Interface
 *
 * This file defines the SPI C++ interface for libmaa
 *
 * @snippet Spi-pot.cpp Interesting
 */
class Spi {
    public:
        /**
         * Initialise SPI object using the board mapping to set muxes
         *
         * @param bus to use, as listed in the platform definition, normally 0
         */
        Spi(int bus) {
            m_spi = maa_spi_init(bus);
        }
        /**
         * Closes spi bus
         */
        ~Spi() {
            maa_spi_stop(m_spi);
        }
        /**
         * Set the SPI device mode. see spidev0-3
         *
         * @param mode the mode. See Linux spidev doc
         * @return Result of operation
         */
        maa_result_t mode(unsigned short mode) {
            return maa_spi_mode(m_spi, mode);
        }
        /**
         * Set the SPI device operating clock frequency
         *
         * @param hz the frequency to set in hz
         * @return Result of operation
         */
        maa_result_t frequency(int hz) {
            return maa_spi_frequency(m_spi, hz);
        }
        /**
         * Write single byte to the SPI device
         *
         * @param data the byte to send
         * @return data received on the miso line
         */
        unsigned char write(uint8_t data) {
            return (unsigned char) maa_spi_write(m_spi, data);
        }
        /**
         * Write buffer of bytes to SPI device
         *
         * @param data buffer to send
         * @param length size of buffer to send
         * @return char* data received on the miso line. Same length as passed in
         */
        unsigned char* write_buf(uint8_t* data, int length) {
            return (unsigned char*) maa_spi_write_buf(m_spi, data, length);
        }
        /**
         * Change the SPI lsb mode
         *
         * @param lsb Use least significant bit transmission - 0 for msbi
         * @return Result of operation
         */
        maa_result_t lsbmode(bool lsb) {
            return maa_spi_lsbmode(m_spi, (maa_boolean_t) lsb);
        }
        /**
         * Set bits per mode on transaction, default is 8
         *
         * @param bits bits per word
         * @return Result of operation
         */
        maa_result_t bit_per_word(unsigned int bits) {
            return maa_spi_bit_per_word(m_spi, bits);
        }
            private:
                maa_spi_context m_spi;
        };
}
