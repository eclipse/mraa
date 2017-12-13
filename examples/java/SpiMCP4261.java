/*
 * Author: Alexander Komarov <alexander.komarov@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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
