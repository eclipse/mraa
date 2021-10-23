/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2017 Linaro Ltd.
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
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "led.h"
#include "types.hpp"
#include <stdexcept>

namespace mraa
{

/**
 * @brief API to Light Emitting Diode
 *
 * This file defines the LED interface for libmraa
 *
 * @snippet led.cpp Interesting
 */
class Led
{
  public:
    /**
     * Instantiates an LED object
     *
     * @param led LED index to use
     */
    Led(int led)
    {
        m_led = mraa_led_init(led);

        if (m_led == NULL) {
            throw std::invalid_argument("Invalid LED name specified");
        }
    }

    /**
     * Instantiates an LED object
     *
     * @param led_dev LED function name to use
     */
    Led(std::string led_dev)
    {
        m_led = mraa_led_init_raw(led_dev.c_str());

        if (m_led == NULL) {
            throw std::invalid_argument("Invalid LED name specified");
        }
    }

    /**
     * LED Constructor, takes a pointer to a LED context and initialises
     * the LED class
     *
     * @param led_context void * to LED context
     */
    Led(void* led_context)
    {
        m_led = (mraa_led_context) led_context;
        if (m_led == NULL) {
            throw std::invalid_argument("Invalid LED name specified");
        }
    }

    /**
     * LED object destructor
     */
    ~Led()
    {
        if (m_led != NULL) {
            mraa_led_close(m_led);
        }
    }

    /*
     * Closes LED explicitly, prior to implicit closing on object destruction
     */
    void
    close()
    {
        mraa_led_close(m_led);
        m_led = NULL;
    }

    /**
     * Set LED brightness value
     *
     * @param value Value to set LED brightness
     * @return Result of operation
     */
    Result
    setBrightness(int value)
    {
        return (Result) mraa_led_set_brightness(m_led, value);
    }

    /**
     * Read LED brightness value
     *
     * @return LED brightness value
     */
    int
    readBrightness()
    {
        return mraa_led_read_brightness(m_led);
    }

    /**
     * Read LED maximum brightness value
     *
     * @return LED max brightness value
     */
    int
    readMaxBrightness()
    {
        return mraa_led_read_max_brightness(m_led);
    }

    /**
     * Set LED trigger
     *
     * @param trigger Type of trigger to set
     * @return Result of operation
     */
    Result
    trigger(const char* trigger)
    {
        return (Result) mraa_led_set_trigger(m_led, trigger);
    }

    /**
     * Clear active LED trigger
     *
     * @return Result of operation
     */
    Result
    clearTrigger()
    {
        return (Result) mraa_led_clear_trigger(m_led);
    }

  private:
    mraa_led_context m_led;
};
}
