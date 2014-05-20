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
 * This file defines the pwm C++ interface for libmaa
 *
 */

#include "pwm.h"

namespace maa {

class Pwm {
    public:
        Pwm(int pin, int chipid=-1, bool owner = true) {
            if (chipid == -1)
                m_pwm = maa_pwm_init(pin);
            else
                m_pwm = maa_pwm_init_raw(pin, chipid);
            if (!owner)
                maa_pwm_owner(m_pwm, 0);
        }
        ~Pwm() {
            maa_pwm_close(m_pwm);
        }
        maa_result_t write(float percentage) {
            return maa_pwm_write(m_pwm, percentage);
        }
        float read() {
            return maa_pwm_read(m_pwm);
        }
        maa_result_t period(float period) {
            return maa_pwm_period(m_pwm, period);
        }
        maa_result_t period_ms(int ms) {
            return maa_pwm_period_ms(m_pwm, ms);
        }
        maa_result_t period_us(int us) {
            return maa_pwm_period_us(m_pwm, us);
        }
        maa_result_t pulsewidth(float seconds) {
            return maa_pwm_pulsewidth(m_pwm, seconds);
        }
        maa_result_t pulsewidth_ms(int ms) {
            return maa_pwm_pulsewidth_ms(m_pwm, ms);
        }
        maa_result_t pulsewidth_us(int us) {
            return maa_pwm_pulsewidth_us(m_pwm, us);
        }
        maa_result_t enable(bool enable) {
            if (enable)
                return maa_pwm_enable(m_pwm, 1);
            else
                return maa_pwm_enable(m_pwm, 0);
        }
    private:
        maa_pwm_context m_pwm;
};

}
