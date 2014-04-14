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

#include "pwm.h"

using namespace maa;

PWM::PWM(int chipin, int pinin)
{
    chipid = chipin;
    pin = pinin;

    FILE *export_f;
    char buffer[64];
    snprintf(buffer, 64, "/sys/class/pwm/pwmchip%d/export", chipid);

    if((export_f = fopen(buffer, "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
    } else {
        fprintf(export_f, "%d", pin);
        fclose(export_f);
    }
}

void
PWM::write(float percentage)
{
    //DO some writting
}

float
PWM::read()
{
    //Do Something
}

void
PWM::period(float seconds)
{
    //Do Something
}

void
PWM::period_ms(int ms)
{
    //Do Something
}

void
PWM::perod_us(int us)
{
    //Do Something
}

void
PWM::pulsewidth(float seconds)
{
    //Do Something
}

void
PWM::pulsewidth_ms(int ms)
{
    //Do Something
}

void
PWM::pulsewidth_us(int us)
{
    //Do Something
}

void
PWM::enable(int enable)
{
    //Do Something
}

void
PWM::close()
{
    //Do Something
}

void
PWM::write_period(int period)
{
    //Impossible
}

void
PWM::write_duty(int duty)
{
    if(duty_fp == NULL) {
        setup_duty_fp();
    }
    fseek(duty_fp, SEEK_SET, 0);
    fprintf(duty_fp, "%d", duty);
    fseek(duty_fp, SEEK_SET, 0);
}

int
PWM::setup_duty_fp()
{
    char bu[64];
    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chipid, pin);

    if((duty_fp = fopen(bu, "r+b")) == NULL) {
        return 1;
    } else {
        return 0;
    }
    return 1;
}
