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

#include "gpio.h"
#include <stdexcept>

namespace mraa {

// These enums must match the enums in gpio.h

/**
 * Gpio Output modes
 */
typedef enum {
    MODE_STRONG   = 0, /**< Default. Strong High and Low */
    MODE_PULLUP   = 1, /**< Interupt on rising & falling */
    MODE_PULLDOWN = 2, /**< Interupt on rising only */
    MODE_HIZ      = 3  /**< Interupt on falling only */
} Mode;

/**
 * Gpio Direction options
 */
typedef enum {
    DIR_OUT = 0, /**< Output. A Mode can also be set */
    DIR_IN  = 1  /**< Input */
} Dir;

/**
 * Gpio Edge types for interupts
 */
typedef enum {
    EDGE_NONE    = 0, /**< No interrupt on Gpio */
    EDGE_BOTH    = 1, /**< Interupt on rising & falling */
    EDGE_RISING  = 2, /**< Interupt on rising only */
    EDGE_FALLING = 3  /**< Interupt on falling only */
} Edge;

/**
 * @brief API to General Purpose IO
 *
 * This file defines the gpio interface for libmraa
 *
 * @snippet Blink-IO.cpp Interesting
 */
class Gpio {
    public:
        /**
         * Instanciates a Gpio object
         *
         * @param pin pin number to use
         * @param owner (optional) Set pin owner, default behaviour is to 'own'
         * the pin if we exported it. This means we will close it on destruct.
         * Otherwise it will get left open. This is only valid in sysfs use
         * cases
         * @param raw (optional) Raw pins will use gpiolibs pin numbering from
         * the kernel module. Note that you will not get any muxers set up for
         * you so this may not always work as expected.
         */
        Gpio(int pin, bool owner=true, bool raw=false) {
            if (raw) {
                m_gpio = mraa_gpio_init_raw(pin);
            }
            else {
                m_gpio = mraa_gpio_init(pin);
            }

            if (m_gpio == NULL) {
                throw std::invalid_argument("Invalid GPIO pin specified");
            }

            if (!owner) {
                mraa_gpio_owner(m_gpio, 0);
            }
        }
        /**
         * Gpio object destructor, this will only unexport the gpio if we where
         * the owner
         */
        ~Gpio() {
            mraa_gpio_close(m_gpio);
        }
        /**
         * Set the edge mode for ISR
         *
         * @param mode The edge mode to set
         * @return Result of operation
         */
        mraa_result_t edge(Edge mode) {
            return mraa_gpio_edge_mode(m_gpio, (gpio_edge_t) mode);
        }
#if defined(SWIGPYTHON)
        mraa_result_t isr(Edge mode, PyObject *pyfunc, PyObject* args) {
            return mraa_gpio_isr(m_gpio, (gpio_edge_t) mode, (void (*) (void *)) pyfunc, (void *) args);
        }
#else
        /**
         * Sets a callback to be called when pin value changes
         *
         * @param mode The edge mode to set
         * @param fptr Function pointer to function to be called when interupt is
         * triggered
         * @param args Arguments passed to the interrupt handler (fptr)
         * @return Result of operation
         */
        mraa_result_t isr(Edge mode, void (*fptr)(void *), void * args) {
            return mraa_gpio_isr(m_gpio, (gpio_edge_t) mode, fptr, args);
        }
#endif
        /**
         * Exits callback - this call will not kill the isr thread imediatlu
         * but only when it is out of it's critical section
         *
         * @return Result of operation
         */
        mraa_result_t isrExit() {
            return mraa_gpio_isr_exit(m_gpio);
        }
        /**
         * Change Gpio mode
         *
         * @param mode The mode to change the gpio into
         * @return Result of operation
         */
        mraa_result_t mode(Mode mode) {
            return mraa_gpio_mode(m_gpio, (gpio_mode_t) mode);
        }
        /**
         * Change Gpio direction
         *
         * @param dir The direction to change the gpio into
         * @return Result of operation
         */
        mraa_result_t dir(Dir dir) {
            return mraa_gpio_dir(m_gpio, (gpio_dir_t) dir);
        }
        /**
         * Read value from Gpio
         *
         * @return Gpio value
         */
        int read() {
            return mraa_gpio_read(m_gpio);
        }
        /**
         * Write value to Gpio
         *
         * @param value Value to write to Gpio
         * @return Result of operation
         */
        mraa_result_t write(int value) {
            return mraa_gpio_write(m_gpio, value);
        }
        /**
         * Enable use of mmap i/o if available.
         *
         * @param enable true to use mmap
         * @return Result of operation
         */
        mraa_result_t useMmap(bool enable) {
            return mraa_gpio_use_mmaped(m_gpio, (mraa_boolean_t) enable);
        }
        /**
         * Get pin number of Gpio
         *
         * @return Pin number
         */
        int getPin() {
            return mraa_gpio_get_pin(m_gpio);
        }
    private:
        mraa_gpio_context m_gpio;
};

}
