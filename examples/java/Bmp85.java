/*
 * Author: Alexander Komarov <alexander.komarov@intel.com>
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

public class Bmp85 {
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
  public static void main(String argv[]) {
    mraa.mraa.init();
    System.out.println(mraa.mraa.getVersion());

    // helper function to go from hex val to dec
    // function char(x) { return parseInt(x, 16); }

    mraa.I2c i2c = new mraa.I2c(0);
    i2c.address((byte)0x77);
    i2c.writeByte((byte)0xd0);
    /*
    SWIGTYPE_p_unsigned_char data0 = new SWIGTYPE_p_unsigned_char();*/
    byte[] data = new byte[1];
    i2c.read(data);
    System.out.println((new Integer(data[0])).toString());

    i2c.writeReg((byte)0xf4, (byte)0x2e);
    // initialise device
    if (i2c.readReg((byte)0xd0) != 0x55) {
      System.out.println("error");
    }

    // we want to read temperature so write 0x2e into control reg
    i2c.writeReg((byte)0xf4, (byte)0x2e);

    // read a 16bit reg, obviously it's uncalibrated so mostly a useless value
    // :)
    System.out.println(i2c.readWordReg((byte)0xf6));

    byte[] buf = new byte[2];
    buf[0] = (byte)0xf4;
    buf[1] = (byte)0x2e;
    i2c.write(buf);

    i2c.writeByte((byte)0xf6);
    int d = i2c.readReg((byte)2);
    System.out.println((new Integer(d)).toString());
  };
}
;
