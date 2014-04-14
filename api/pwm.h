/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 *
 * Copyright © 2014 Intel Corporation
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

#include <stdio.h>
#include <fcntl.h>

namespace maa {

/** A PWM object, used for interacting with PWM output.
 *
 * Example:
 * @code
 * // Set up PWM object then cycle percentage 0-100.
 *
 * #include "maa.h"
 *
 * PWM pwm(3);
 *
 * int main() {
 *     pwm.period_us(7968750i); //Max Galileo Rev D
 *     pwm.enable(1);
 *
 *     float value = 0;
 *     while(1) {
 *         pwm.write(value);
 *         sleep(0.5);
 *         if(value == 1.0) {
 *             value = 0;
 *         } else {
 *             value = value +0.1;
 *         }
 *     }
 * }
 * @endcode
 */
class PWM {

private:
    int chipid, pin;
    FILE *duty_fp;

    void write_period(int period);
    void write_duty(int duty);
    int setup_duty_fp();
    int get_period();
    int get_duty();

public:

    /** Create an PWM object
     *
     *  @param chipid The chip in which the following pin is on.
     *  @param pin The PWM channel to operate on
     */
    PWM(int chipid, int pin);

    /** Set the ouput duty-cycle percentage, as a float
     *
     *  @param percentage A floating-point value representing percentage of output.
     *    The value should lie between 0.0f (representing on 0%) and 1.0f
     *    Values above or below this range will be set at either 0.0f or 1.0f.
     */
    void write(float percentage);

    /** Read the ouput duty-cycle percentage, as a float
     *
     *  @return percentage A floating-point value representing percentage of output.
     *    The value should lie between 0.0f (representing on 0%) and 1.0f
     *    Values above or below this range will be set at either 0.0f or 1.0f.
     */
    float read();

    /** Set the PWM period as seconds represented in a float
     *
     *  @param seconds Peroid represented as a float in seconds.
     */
    void period(float seconds);

    /** Set period. milli-oseconds.
     *  @param ms milli-seconds for period.
     */
    void period_ms(int ms);

    /** Set period. microseconds
     *  @param ns microseconds as period.
     */
    void period_us(int us);

    /** Set pulsewidth, As represnted by seconds in a (float).
     *  @param seconds The duration of a pulse
     */
    void pulsewidth(float seconds);

     /** Set pulsewidth. Milliseconds
     *  @param ms milliseconds for pulsewidth.
     */
    void pulsewidth_ms(int ms);

      /** Set pulsewidth, microseconds.
     *  @param us microseconds for pulsewidth.
     */
    void pulsewidth_us(int us);

    /** Set the enable status of the PWM pin. None zero will assume on with output being driven.
     *   and 0 will disable the output.
     *  @param enable enable status of pin
     */
    void enable(int enable);

     /** Close and unexport the PWM pin.
     */
    void close();

};
}
