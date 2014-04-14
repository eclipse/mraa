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

#include <stdlib.h>

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
        setup_duty_fp();
    }
}

void
PWM::write(float percentage)
{
    write_duty(percentage*get_period());
}

float
PWM::read()
{
    float output = get_duty() / (float) get_period();
    return output;
}

void
PWM::period(float seconds)
{
    period_ms(seconds*1000);
}

void
PWM::period_ms(int ms)
{
    period_us(ms*1000);
}

void
PWM::period_us(int us)
{
    write_period(us*1000);
}

void
PWM::pulsewidth(float seconds)
{
    pulsewidth_ms(seconds*1000);
}

void
PWM::pulsewidth_ms(int ms)
{
    pulsewidth_us(ms*1000);
}

void
PWM::pulsewidth_us(int us)
{
    write_duty(us*1000);
}

void
PWM::enable(int enable)
{
    int status;
    if(enable != 0) {
        status = 1;
    } else {
        status = enable;
    }
    FILE *enable_f;
    char bu[64];
    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chipid, pin);

    if((enable_f = fopen(bu, "w")) == NULL) {
        fprintf(stderr, "Failed to open export for writing!\n");
    } else {
        fprintf(enable_f, "%d", status);
        fclose(enable_f);
    }
    //Do Something
}

void
PWM::close()
{
    enable(0);
    FILE *unexport_f;
    char buffer[64];
    snprintf(buffer, 64, "/sys/class/pwm/pwmchip%d/unexport", chipid);

    if((unexport_f = fopen(buffer, "w")) == NULL) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
    } else {
        fprintf(unexport_f, "%d", pin);
        fclose(unexport_f);
    }
}

void
PWM::write_period(int period)
{
    FILE *period_f;
    char bu[64];
    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/period", chipid, pin);

    if((period_f = fopen(bu, "r+b")) == NULL) {
        fprintf(stderr, "Failed to open period for writing!\n");
    } else {
        fprintf(period_f, "%d", period);
        fclose(period_f);
    }
}

void
PWM::write_duty(int duty)
{
    if(duty_fp == NULL) {
        setup_duty_fp();
    }
    fprintf(duty_fp, "%d", duty);
    rewind(duty_fp);
    fflush(duty_fp);
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

int
PWM::get_period()
{
    FILE *period_f;
    char bu[64];
    char output[16];

    sprintf(bu, "/sys/class/pwm/pwmchip%d/pwm%d/period", chipid, pin);
    if((period_f = fopen(bu, "rb")) == NULL) {
        fprintf(stderr, "Failed to open period for reading!\n");
        return 0;
    } else {
        fgets(output, 16, period_f);
        fclose(period_f);
        return atoi(output);
    }
}

int
PWM::get_duty()
{
    if(duty_fp == NULL) {
        setup_duty_fp();
    }
    char output[16];
    fgets(output, 16, duty_fp);
    fseek(duty_fp, SEEK_SET, 0);
    return atoi(output);
}
