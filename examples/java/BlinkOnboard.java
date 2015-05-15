/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Jakub Kramarz <jkramarz@virtuslab.com>
 * Copyright (c) 2015 VirtusLab
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

public class BlinkOnboard {
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
    mraa.mraa_platform_t platform = mraa.mraa.getPlatformType();
    mraa.Gpio gpio, gpio_in;
    if (platform == mraa.mraa_platform_t.INTEL_GALILEO_GEN1) {
      gpio = new mraa.Gpio(3);
    } else if (platform == mraa.mraa_platform_t.INTEL_MINNOWBOARD_MAX) {
      gpio = new mraa.Gpio(21);
    } else {
      gpio = new mraa.Gpio(13);
    }
    System.out.format("Welcome to libmraa\n Version: %s\n Running on %s\n",
                      mraa.mraa.getVersion(), platform.toString());

    gpio.dir(mraa.Dir.DIR_OUT);
    // on platforms with physical button use gpio_in
    if (platform == mraa.mraa_platform_t.INTEL_MINNOWBOARD_MAX) {
      gpio_in = new mraa.Gpio(14);
      gpio_in.dir(mraa.Dir.DIR_IN);
      System.out.println("Press and hold S1 to stop, Press SW1 to shutdown!");
    } else {
      gpio_in = null;
    }

    boolean state = false;
    while (true) {
      if (gpio_in != null && gpio_in.read() == 0) {
        return;
      }
      if (state) {
        state = false;
        gpio.write(1);
      } else {
        state = true;
        gpio.write(0);
      }
      Thread.sleep(1000);
    }
  }
}