/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
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


import mraa.Dir;
import mraa.Gpio;
import mraa.Result;
import mraa.mraa;

public class BlinkIO {
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

    final static int DEFAULT_IOPIN = 8;

    public static void main(String argv[]) throws InterruptedException {
        int iopin = DEFAULT_IOPIN;
        if (argv.length == 0) {
            System.out.println("Provide an int arg if you want to flash on something other than " + DEFAULT_IOPIN);
        } else {
            iopin = Integer.valueOf(argv[0]);
        }

        //! [Interesting]
        Gpio gpio = new Gpio(iopin);
        Result result = gpio.dir(Dir.DIR_OUT);
        if (result != Result.SUCCESS) {
            mraa.printError(result);
            System.exit(1);
        }

        for (int i = 100; i > 0; --i) {
            gpio.write(1);
            Thread.sleep(1000);
            gpio.write(0);
            Thread.sleep(1000);
        }
        //! [Interesting]
    }
}