/*
 * Author: Alexander Komarov <alexander.komarov@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

import mraa.Spi;

public class SpiMCP4261 {
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
    public static void main(String argv[]) throws InterruptedException {
        //! [Interesting]
        Spi spi = new Spi(0);

        System.out.println("Hello, SPI initialised");
        byte data[] = {0x00, 100};
        for (int cnt = 100; cnt > 0; --cnt) {
            for (int i = 90; i < 130; i++) {
                data[1] = (byte) i;
                byte[] recv = spi.write(data);
                System.out.println(String.format("Writing - %d", i));
                System.out.println(String.format("Received - %d - %d", recv[0], recv[1]));
                Thread.sleep(100);
            }
            for (int i = 130; i > 90; i--) {
                data[1] = (byte) i;
                byte[] recv = spi.write(data);
                System.out.println(String.format("Writing - %d", i));
                System.out.println(String.format("Received - %d - %d", recv[0], recv[1]));
                Thread.sleep(100);
            }
        }
        //! [Interesting]
    };
}
