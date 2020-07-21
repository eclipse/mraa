/*
 * Author: Nandkishor Sonar
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */


//! [Interesting]
import mraa.Aio;

public class AioA0 {

    static {
        try {
            System.loadLibrary("mraajava");
        } catch (UnsatisfiedLinkError e) {
            System.err.println(
                "Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" +
                    e);
            System.exit(1);
        }
    }

    public static void main(String[] args) {
        Aio a0 = new Aio(0);

        for (int i = 100; i > 0; --i) {
            int adc_value = a0.read();
            float adc_value_float = a0.readFloat();
            System.out.println(String.format("ADC A0 read %X - %d", adc_value, adc_value));
            System.out.println(String.format("ADC A0 read %.5f", adc_value_float));
            Thread.sleep(500);
        }

    }
}
//! [Interesting]
