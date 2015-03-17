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
#include <stdexcept>

namespace mraa
{

/**
 * MRAA SPI Modes
 */
typedef enum {
    SPI_MODE0 = 0, /**< CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge,
                      output data (change) on falling edge */
    SPI_MODE1 = 1, /**< CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge,
                      output data (change) on rising edge */
    SPI_MODE2 = 2, /**< CPOL = 1, CPHA = 0, Clock idle low, data is clocked in on falling edge,
                      output data (change) on rising edge */
    SPI_MODE3 = 3, /**< CPOL = 1, CPHA = 1, Clock idle low, data is clocked in on rising, edge
                      output data (change) on falling edge */
} Spi_Mode;

typedef enum {
    MRAA_SPI_SOFT_8BIS = 8,   //** SoftSPI handles CS pin in chunks of 8 Bits */
    MRAA_SPI_SOFT_9BIS = 9,   //** SoftSPI handles CS pin in chunks of 9 Bits (if spidev associated
                              //allows 9 bit transfer) */
    MRAA_SPI_SOFT_16BIS = 16, //** SoftSPI handles CS pin in chunks of 16 bits */
} Spi_Soft_Bits;

/**
 * @brief API to Serial Peripheral Interface
 *
 * This file defines the SPI interface for libmraa
 *
 * @snippet Spi-pot.cpp Interesting
 */
class Spi
{
  public:
    /**
     * Initialise SPI object using the board mapping to set muxes
     *
     * @param bus to use, as listed in the platform definition, normally 0
     */
    Spi(int bus)
    {
        m_spi = mraa_spi_init(bus);

        if (m_spi == NULL) {
            throw std::invalid_argument("Error initialising SPI bus");
        }
    }

    /**
     * Closes spi bus
     */
    ~Spi()
    {
        mraa_spi_stop(m_spi);
    }

    /**
     * Set the SPI device mode. see spidev0-3
     *
     * @param mode the mode. See Linux spidev doc
     * @return Result of operation
     */
    mraa_result_t
    mode(Spi_Mode mode)
    {
        return mraa_spi_mode(m_spi, (mraa_spi_mode_t) mode);
    }

    /**
     * Set the SPI device operating clock frequency
     *
     * @param hz the frequency to set in hz
     * @return Result of operation
     */
    mraa_result_t
    frequency(int hz)
    {
        return mraa_spi_frequency(m_spi, hz);
    }

    /**
     * Write single byte to the SPI device
     *
     * @param data the byte to send
     * @return data received on the miso line or -1 in case of error
     */
    int
    writeByte(uint8_t data)
    {
        return mraa_spi_write(m_spi, (uint8_t) data);
    }

    /**
     * Write single byte to the SPI device
     *
     * @param data the byte to send
     * @return data received on the miso line
     */
    uint16_t
    write_word(uint16_t data)
    {
        return mraa_spi_write_word(m_spi, (uint16_t) data);
    }

    /**
     * Write buffer of bytes to SPI device The pointer return has to be
     * free'd by the caller. It will return a NULL pointer in cases of
     * error
     *
     * @param txBuf buffer to send
     * @param length size of buffer to send
     * @return uint8_t* data received on the miso line. Same length as passed in
     */
    uint8_t*
    write(uint8_t* txBuf, int length)
    {
        return mraa_spi_write_buf(m_spi, txBuf, length);
    }

    /**
     * Write buffer of bytes to SPI device The pointer return has to be
     * free'd by the caller. It will return a NULL pointer in cases of
     * error
     *
     * @param txBuf buffer to send
     * @param length size of buffer (in bytes) to send
     * @return uint8_t* data received on the miso line. Same length as passed in
     */
    uint16_t*
    write_word(uint16_t* txBuf, int length)
    {
        return mraa_spi_write_buf_word(m_spi, txBuf, length);
    }

#ifndef SWIG
    /**
     * Transfer data to and from SPI device Receive pointer may be null if
     * return data is not needed.
     *
     * @param data buffer to send
     * @param rxBuf buffer to optionally receive data from spi device
     * @param length size of buffer to send
     * @return Result of operation
     */
    mraa_result_t
    transfer(uint8_t* txBuf, uint8_t* rxBuf, int length)
    {
        return mraa_spi_transfer_buf(m_spi, txBuf, rxBuf, length);
    }

    /**
     * Transfer data to and from SPI device Receive pointer may be null if
     * return data is not needed.
     *
     * @param data buffer to send
     * @param rxBuf buffer to optionally receive data from spi device
     * @param length size of buffer to send
     * @return Result of operation
     */
    mraa_result_t
    transfer_word(uint16_t* txBuf, uint16_t* rxBuf, int length)
    {
        return mraa_spi_transfer_buf_word(m_spi, txBuf, rxBuf, length);
    }
#endif

    /**
     * Change the SPI lsb mode
     *
     * @param lsb Use least significant bit transmission - 0 for msbi
     * @return Result of operation
     */
    mraa_result_t
    lsbmode(bool lsb)
    {
        return mraa_spi_lsbmode(m_spi, (mraa_boolean_t) lsb);
    }

    /**
     * Disables the hardware chip select on the platform that is toggled by spidev
     * effectively giving you a handle on the raw spi bus
     *
     * @param hwcs Disable/Enable hardware chip select
     * @return Result of operation
     */
    mraa_result_t
    hwcs(bool hwcs)
    {
        return mraa_spi_hw_cs(m_spi, (mraa_boolean_t) hwcs);
    }

    /**
     * Disables the hardware chip select on the platform that is toggled by spidev
     * and replaces it with a software based chip select managed by mraa to
     * overcome limitations of the implementation on some platforms supporting only
     * 8 or 9 bit transfers do not use this on platforms that have native support
     * for your required bitlength as spi performance will suffer using soft chip
     * select
     *
     * @param number of bits per chip select. Only multiples of existing number of bits are
     *supported
     * @param replacement cs pin (can be any valid mraa Gpio)
     * @return Result of operation
     */
    mraa_result_t
    swcs(Spi_Soft_Bits softbits, int pin)
    {
        return mraa_spi_sw_cs(m_spi, (mraa_spi_soft_bits_t) softbits, pin);
    }

    /**
     * Set bits per mode on transaction, default is 8
     *
     * @param bits bits per word
     * @return Result of operation
     */
    mraa_result_t
    bitPerWord(unsigned int bits)
    {
        return mraa_spi_bit_per_word(m_spi, bits);
    }

  private:
    mraa_spi_context m_spi;
};
}
