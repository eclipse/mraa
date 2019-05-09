/*
 * Author: Mihai Stefanescu <mihai.stefanescu@rinftech.com>
 * Copyright (c) 2018 Intel Corporation.
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

#include "initio.h"
#include <stdexcept>
#include <string>
#include <vector>

#include "aio.hpp"
#include "gpio.hpp"
#include "i2c.hpp"

#if !defined(PERIPHERALMAN)
#include "iio.hpp"
#endif

#include "pwm.hpp"
#include "spi.hpp"
#include "uart.hpp"
#include "uart_ow.hpp"

namespace mraa
{
class MraaIo
{
  private:
    mraa_io_descriptor* descs = nullptr;

  public:
    MraaIo(const std::string& initStr) : descs()
    {
        if (mraa_io_init(initStr.c_str(), &descs) != MRAA_SUCCESS) {
            throw std::runtime_error("mraa_io_init error");
        }

        aios.reserve(descs->n_aio);
        for (int i = 0; i < descs->n_aio; ++i) {
            aios.emplace_back(descs->aios[i]);
        }

        gpios.reserve(descs->n_gpio);
        for (int i = 0; i < descs->n_gpio; ++i) {
            gpios.emplace_back(descs->gpios[i]);
        }

        i2cs.reserve(descs->n_i2c);
        for (int i = 0; i < descs->n_i2c; ++i) {
            i2cs.emplace_back(descs->i2cs[i]);
        }

#if !defined(PERIPHERALMAN)
        iios.reserve(descs->n_iio);
        for (int i = 0; i < descs->n_iio; ++i) {
            iios.emplace_back(descs->iios[i]);
        }
#endif

        pwms.reserve(descs->n_pwm);
        for (int i = 0; i < descs->n_pwm; ++i) {
            pwms.emplace_back(descs->pwms[i]);
        }

        spis.reserve(descs->n_spi);
        for (int i = 0; i < descs->n_spi; ++i) {
            spis.emplace_back(descs->spis[i]);
        }

        uarts.reserve(descs->n_uart);
        for (int i = 0; i < descs->n_uart; ++i) {
            uarts.emplace_back(descs->uarts[i]);
        }

        uart_ows.reserve(descs->n_uart_ow);
        for (int i = 0; i < descs->n_uart_ow; ++i) {
            uart_ows.emplace_back(descs->uart_ows[i]);
        }

        if (descs->leftover_str) {
            leftoverStr = std::string(descs->leftover_str);
        } else {
            leftoverStr = std::string("");
        }
    }

    MraaIo() : descs() {}

    ~MraaIo()
    {
        if (descs != nullptr) {
            if (descs->leftover_str) {
                free(descs->leftover_str);
            }
            if (descs->n_aio) {
                free(descs->aios);
            }
            if (descs->n_gpio) {
                free(descs->gpios);
            }
            if (descs->n_i2c) {
                free(descs->i2cs);
            }
#if !defined(PERIPHERALMAN)
            if (descs->n_iio) {
                free(descs->iios);
            }
#endif
            if (descs->n_pwm) {
                free(descs->pwms);
            }
            if (descs->n_spi) {
                free(descs->spis);
            }
            if (descs->n_uart) {
                free(descs->uarts);
            }
            if (descs->n_uart_ow) {
                free(descs->uart_ows);
            }

            /* Finally free the mraa_io_descriptor structure. */
            free(descs);
        }
    }

  public:
    std::vector<Aio> aios;
    std::vector<Gpio> gpios;
    std::vector<I2c> i2cs;
#if !defined(PERIPHERALMAN)
    std::vector<Iio> iios;
#endif
    std::vector<Pwm> pwms;
    std::vector<Spi> spis;
    std::vector<Uart> uarts;
    std::vector<UartOW> uart_ows;

  private:
    /* Used exclusively by the UPM library. */
    std::string leftoverStr;

  public:
    /* This is used mainly by sensors that use C structs/functions in C++ code. */
    mraa_io_descriptor*
    getMraaDescriptors()
    {
        return descs;
    }

    std::string
    getLeftoverStr()
    {
        return leftoverStr;
    }
};
}
