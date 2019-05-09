/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

import mraa.I2c;

public class I2cCompass {
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

    final static byte MAX_BUFFER_LENGTH = 6;
    final static byte CONF_BUFFER_LENGTH = 2;
    final static short HMC5883L_I2C_ADDR = 0x1E;

    // configuration registers
    final static byte HMC5883L_CONF_REG_A = 0x00;
    final static byte HMC5883L_CONF_REG_B = 0x01;

    // mode register
    final static byte HMC5883L_MODE_REG = 0x02;

    // data register
    final static byte HMC5883L_X_MSB_REG = 0;
    final static byte HMC5883L_X_LSB_REG = 1;
    final static byte HMC5883L_Z_MSB_REG = 2;
    final static byte HMC5883L_Z_LSB_REG = 3;
    final static byte HMC5883L_Y_MSB_REG = 4;
    final static byte HMC5883L_Y_LSB_REG = 5;
    final static byte DATA_REG_SIZE = 6;

    // status register
    final static byte HMC5883L_STATUS_REG = 0x09;

    // ID registers
    final static byte HMC5883L_ID_A_REG = 0x0A;
    final static byte HMC5883L_ID_B_REG = 0x0B;
    final static byte HMC5883L_ID_C_REG = 0x0C;

    final static byte HMC5883L_CONT_MODE = 0x00;
    final static byte HMC5883L_DATA_REG = 0x03;

    // scales
    final static byte GA_0_88_REG = ((byte) (0x00 << 5));
    final static byte GA_1_3_REG = ((byte) (0x01 << 5));
    final static byte GA_1_9_REG = ((byte) (0x02 << 5));
    final static byte GA_2_5_REG = ((byte) (0x03 << 5));
    final static byte GA_4_0_REG = ((byte) (0x04 << 5));
    final static byte GA_4_7_REG = ((byte) (0x05 << 5));
    final static byte GA_5_6_REG = ((byte) (0x06 << 5));
    final static byte GA_8_1_REG = ((byte) (0x07 << 5));

    // digital resolutions
    final static float SCALE_0_73_MG = 0.73f;
    final static float SCALE_0_92_MG = 0.92f;
    final static float SCALE_1_22_MG = 1.22f;
    final static float SCALE_1_52_MG = 1.52f;
    final static float SCALE_2_27_MG = 2.27f;
    final static float SCALE_2_56_MG = 2.56f;
    final static float SCALE_3_03_MG = 3.03f;
    final static float SCALE_4_35_MG = 4.35f;

    public static void main(String[] args) throws InterruptedException {
        float direction = 0;
        int x, y, z;
        byte[] rx_tx_buf = new byte[MAX_BUFFER_LENGTH];
        byte[] conf_buf = new byte[CONF_BUFFER_LENGTH];

        //! [Interesting]
        I2c i2c = new I2c(0);

        i2c.address(HMC5883L_I2C_ADDR);
        conf_buf[0] = HMC5883L_CONF_REG_B;
        conf_buf[1] = GA_1_3_REG;
        i2c.write(conf_buf);
        //! [Interesting]

        i2c.address(HMC5883L_I2C_ADDR);
        conf_buf[0] = HMC5883L_MODE_REG;
        conf_buf[1] = HMC5883L_CONT_MODE;
        i2c.write(conf_buf);

        for (int i = 100; i > 0; --i) {
            i2c.address(HMC5883L_I2C_ADDR);
            i2c.writeByte(HMC5883L_DATA_REG);

            i2c.address(HMC5883L_I2C_ADDR);
            i2c.read(rx_tx_buf);

            x = (rx_tx_buf[HMC5883L_X_MSB_REG] << 8) | (rx_tx_buf[HMC5883L_X_LSB_REG] & 0xFF);
            z = (rx_tx_buf[HMC5883L_Z_MSB_REG] << 8) | (rx_tx_buf[HMC5883L_Z_LSB_REG] & 0xFF);
            y = (rx_tx_buf[HMC5883L_Y_MSB_REG] << 8) | (rx_tx_buf[HMC5883L_Y_LSB_REG] & 0xFF);

            direction = (float) Math.atan2(y * SCALE_0_92_MG, x * SCALE_0_92_MG);

            if (direction < 0)
                direction += 2 * Math.PI;

            System.out.println(String.format("Compass scaled data x : %f, y : %f, z : %f\n", x * SCALE_0_92_MG, y * SCALE_0_92_MG,
                    z * SCALE_0_92_MG));
            System.out.println(String.format("Heading : %f\n", direction * 180 / Math.PI));
            Thread.sleep(1000);
        }
    }
}
