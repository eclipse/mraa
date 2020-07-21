/*
 * Author: Michael Ring <mail@michael-ring.org>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

import java.nio.ByteBuffer;
import mraa.Result;
import mraa.Spi;

public class SpiMAX7219 {
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
        Spi spi = new Spi(1);

        spi.frequency(400000);
        spi.lsbmode(false);

        if(spi.bitPerWord(16) != Result.SUCCESS) {
            System.err.println("Could not set SPI Device to 16Bit mode, exit...");
            System.exit(1);
        }

        spi.write_word(0x0900); //Do not decode bits
        spi.write_word(0x0a05); // Brightness of LEDs
        spi.write_word(0x0b07); // Show all Scan Lines
        spi.write_word(0x0c01); // Display on
        spi.write_word(0x0f00); // Testmode off

        short dataAA55[] = { 0x01aa, 0x0255, 0x03aa, 0x0455, 0x05aa, 0x0655, 0x07aa, 0x0855 };
        ByteBuffer buf = ByteBuffer.allocate(dataAA55.length * 2);
        for (int i = 0; i < dataAA55.length; i++)
            buf.putShort(dataAA55[i]);

        spi.write(buf.array());
        Thread.sleep(2000);

        short data55AA[] = { 0x0155, 0x02aa, 0x0355, 0x04aa, 0x0555, 0x06aa, 0x0755, 0x08aa };
        buf = ByteBuffer.allocate(data55AA.length * 2);
        for (int i = 0; i < data55AA.length; i++)
            buf.putShort(data55AA[i]);

        spi.write(buf.array());
        Thread.sleep(2000);

        short data[] = { 0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700, 0x0800 };
        buf = ByteBuffer.allocate(data.length * 2);
        for (int i = 0; i < data.length; i++)
            buf.putShort(data[i]);

        spi.write(buf.array());

        for (int i = 1; i <= 8; i++) {
            for (int j = 0; j < 8; j++) {
                spi.write_word((i << 8) + (1 << j));
                Thread.sleep(1000);
            }
            spi.write_word(i << 8);
        }
        //! [Interesting]
    };
}
