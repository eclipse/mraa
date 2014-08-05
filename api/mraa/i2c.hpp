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

#include "i2c.h"

namespace mraa {

/**
 * @brief C++ API to Inter-Integrated Circuit
 *
 * This file defines the I2c C++ interface for libmraa
 *
 * @snippet I2c-compass.cpp Interesting
 */
class I2c {
    public:
        /**
         * Instantiates an i2c bus. Multiple instances of the same bus can
         * exist and the bus is not guarranteed to be on the correct address
         * before read/write.
         *
         * @param bus The i2c bus to use
         * @param raw Whether to disable pinmapper for your board
         */
        I2c(int bus, bool raw=false) {
            if (raw)
                m_i2c = mraa_i2c_init_raw(bus);
            else
                m_i2c = mraa_i2c_init(bus);
        }
        /**
         * Closes the I2c Bus used. This does not guarrantee the bus will not
         * be usable by anyone else or communicates this disconnect to any
         * slaves.
         */
        ~I2c() {
            mraa_i2c_stop(m_i2c);
        }
        /**
         * Sets the i2c Frequency for communication. Your board may not support
         * the set frequency. Anyone can change this at any time and this will
         * affect every slave on the bus
         *
         * @param hz Frequency to set the bus to in hz
         * @return Result of operation
         */
        mraa_result_t frequency(int hz) {
            return mraa_i2c_frequency(m_i2c, hz);
        }
        /**
         * Set the slave to talk to, typically called before every read/write
         * operation
         *
         * @param address Communicate to the i2c slave on this address
         * @return Result of operation
         */
        mraa_result_t address(int address) {
            return mraa_i2c_address(m_i2c, address);
        }
        /**
         * Read exactly one byte from the bus
         *
         * @return char read from the bus
         */
        unsigned char read() {
            return (unsigned char) mraa_i2c_read_byte(m_i2c);
        }
        /**
         * Read mutliple bytes from the bus
         *
         * @param data Buffer to write into
         * @param length Size of read
         * @return length of the read or 0 if failed
         */
        int read(char *data, size_t length) {
            return mraa_i2c_read(m_i2c, (uint8_t*) data, (int) length);
        }
	/**
	 * Read length bytes from the bus, and return as a std::string note
	 * that this is not a null terminated string
	 *
	 * @param length Size of read to make
	 * @return pointer to std::string
	 */
        std::string read(size_t length) {
            char* data = (char*) malloc(sizeof(char) * length);
            mraa_i2c_read(m_i2c, (uint8_t*) data, (int) length);
            std::string str(data, (int) length);
            free(data);
            return str;
        }
        /**
         * Write one byte to the bus
         *
         * @param data Buffer to send on the bus
         * @param length Size of buffer to send
         * @return Result of operation
         */
        mraa_result_t write(char* data, size_t length) {
            return mraa_i2c_write(m_i2c, (const unsigned char *)data, (int) length);
        }

        /**
         * Write to an i2c register
         *
         * @param reg Register to write to
         * @param data Value to write to register
         * @return Result of operation
         */
        mraa_result_t writeReg(char reg, char data) {
            const unsigned char buf[2] = {(unsigned char) reg, (unsigned char) data};
            return mraa_i2c_write(m_i2c, buf, 2);
        }

        /**
         * Write multiple bytes to the bus
         *
         * @param data The byte to send on the bus
         * @return Result of operation
         */
        mraa_result_t write(char data) {
            return mraa_i2c_write_byte(m_i2c, data);
        }
    private:
        mraa_i2c_context m_i2c;
};

}
