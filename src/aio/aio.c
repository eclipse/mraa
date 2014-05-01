/*
 * Author: Nandkishor Sonar
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

#include <stdlib.h>

#include "aio.h"

static maa_result_t aio_get_valid_fp(maa_aio_context* dev)
{
    char file_path[64]= "";

    //Open file Analog device input channel raw voltage file for reading.
    snprintf(file_path, 64, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw",
        dev->channel );

    if (NULL == (dev->adc_in_fp = fopen(file_path, "r"))) {
	fprintf(stderr, "Failed to open Analog input raw file %s for "
	    "reading!\n", file_path); return( MAA_ERROR_INVALID_RESOURCE);
    }

    return MAA_SUCCESS;
}

/** Configure multiplexer for Analog Input
 *
 * @param aio_channel = Analog input channel to read
 *
 * @return maa_result_t - result type.
 *
 */
static maa_result_t maa_aio_set_mux(unsigned int aio_channel)
{
    maa_result_t result;
    maa_gpio_context* aio_gate;

    //Initialise VINx multiplexer gate pins
    aio_gate = maa_gpio_init(adc_gate_pins[aio_channel]);

    if (NULL == aio_gate) {
        fprintf(stderr, "Failed to initialise first gate pin %d for  ADC "
            "channel %d !\n", adc_gate_pins[aio_channel], aio_channel);
        return MAA_ERROR_INVALID_RESOURCE;
    }

    //Set direction to output for the ADC input gate
    result = maa_gpio_dir(aio_gate, MAA_GPIO_OUT);

    if (MAA_SUCCESS == result) {
        // Write gate configuration output value
        result = maa_gpio_write(aio_gate, 0);

        if (MAA_SUCCESS == result) {
            //For A4 and A5 Analog common gate pin should be high for the
            // Galileo board revision D
            if (A4 == aio_channel || A5 == aio_channel) {
                aio_gate = maa_gpio_init(ADC_COMMON_GATE_A4_A5);

                //Set direction to output for the gate
                if (NULL == aio_gate) {
                    fprintf(stderr, "Failed to initialise second gate pin %d "
                    "for  ADC channel %d !\n", ADC_COMMON_GATE_A4_A5,
                    aio_channel);
                    return(MAA_ERROR_INVALID_RESOURCE);
                }

                result = maa_gpio_dir(aio_gate, MAA_GPIO_OUT);
                if (MAA_SUCCESS == result)
                // Write gate configuration output value
                result = maa_gpio_write(aio_gate, 1);
            }
        }
    }

    if (NULL != aio_gate)
        free(aio_gate);

    return (result);
}

/** Initialise an Analog input, connected to the specified channel
 *
 * @param aio_channel Analog input channel to read
 *
 * @returns pointer to maa_aio_context structure  after initialisation of
 * Analog input pin connected to the device successfully, else returns NULL.
 */
maa_aio_context* maa_aio_init(unsigned int aio_channel)
{
    maa_aio_context* dev;

    // Validate input pins 0-5
    if (aio_channel > TOTAL_ANALOG_INPUTS_ON_BOARD) {
	    fprintf(stderr, "Invalid Analog  input channel %d specified!\n",
	    aio_channel);
        return NULL;
    }

    //Set-up multiplexer for the Analog input channel
    if (MAA_SUCCESS != maa_aio_set_mux(aio_channel)) {
	fprintf(stderr, "Failed to set-up  Analog  input channel %d "
	    "multiplexer!\n", aio_channel); return NULL;
    }

    //Create ADC device connected to specified channel
    dev = (maa_aio_context*) malloc(sizeof(maa_aio_context));
    if (NULL == dev) {
	fprintf(stderr, "Insufficient memory for specified Analog input channel "
	    "%d !\n", aio_channel);
        return NULL;
    }
    dev->channel = aio_channel;

    //Open valid  analog input file and get the pointer.
    if (MAA_SUCCESS != aio_get_valid_fp(dev)) {
        free(dev);
        return NULL;
    }

    return dev;
}

/** Read the input voltage, represented as an unsigned short in the range [0x0,
 * 0xFFFF]
 *
 * @param pointer to maa_aio_context structure  initialised by
 * maa_aio_init()
 *
 * @returns
 *   16-bit unsigned int representing the current input voltage, normalised to
 *   a 16-bit value
 */
unsigned int maa_aio_read_u16(maa_aio_context* dev)
{
    char buffer[16] = "";
    unsigned int raw_value=0;
    unsigned int analog_value=0;
    unsigned int shifter_value=0;

    if (NULL == dev->adc_in_fp) {
        aio_get_valid_fp(dev);
    }

    fseek(dev->adc_in_fp, SEEK_SET, 0);
    fread(buffer, sizeof(buffer), 1, dev->adc_in_fp);
    fseek(dev->adc_in_fp, SEEK_SET, 0);

    raw_value = atoi(buffer);

    /* Adjust the raw analog input reading to supported resolution value*/
    if (ADC_RAW_RESOLUTION_BITS == ADC_SUPPORTED_RESOLUTION_BITS) {
        analog_value = raw_value;
    }
    else {
        if (ADC_RAW_RESOLUTION_BITS > ADC_SUPPORTED_RESOLUTION_BITS) {
            shifter_value = ADC_RAW_RESOLUTION_BITS - ADC_SUPPORTED_RESOLUTION_BITS;
            analog_value =  raw_value >> shifter_value;
        } else {
            shifter_value = ADC_SUPPORTED_RESOLUTION_BITS - ADC_RAW_RESOLUTION_BITS;
            analog_value = raw_value << shifter_value;
        }
    }

    return analog_value;
}

/** Close the analog input and free context memory
 *
 * @param dev - the analog input context
 *
 * @return maa result type.
 */
maa_result_t maa_aio_close(maa_aio_context* dev)
{
    if (NULL != dev)
        free(dev);

    return(MAA_SUCCESS);
}
