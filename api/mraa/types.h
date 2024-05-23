/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

/** @file
 *
 * This file defines the basic shared types for libmraa
 * this file is different to common.h in that swig takes this as an input
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MRAA supported platform types
 */
typedef enum {
    MRAA_INTEL_GALILEO_GEN1 = 0,    /**< The Generation 1 Galileo platform (RevD) */
    MRAA_INTEL_GALILEO_GEN2 = 1,    /**< The Generation 2 Galileo platform (RevG/H) */
    MRAA_INTEL_EDISON_FAB_C = 2,    /**< The Intel Edison (FAB C) */
    MRAA_INTEL_DE3815 = 3,          /**< The Intel DE3815 Baytrail NUC */
    MRAA_INTEL_MINNOWBOARD_MAX = 4, /**< The Intel Minnow Board Max */
    MRAA_RASPBERRY_PI = 5,          /**< The different Raspberry PI Models -like  A,B,A+,B+ */
    MRAA_BEAGLEBONE = 6,            /**< The different BeagleBone Black Modes B/C */
    MRAA_BANANA = 7,                /**< Allwinner A20 based Banana Pi and Banana Pro */
    MRAA_INTEL_NUC5 = 8,            /**< The Intel 5th generations Broadwell NUCs */
    MRAA_96BOARDS = 9,              /**< Linaro 96boards */
    MRAA_INTEL_SOFIA_3GR = 10,      /**< The Intel SoFIA 3GR */
    MRAA_INTEL_CHERRYHILLS = 11,    /**< The Intel Braswell Cherryhills */
    MRAA_UP = 12,                   /**< The UP Board */
    MRAA_INTEL_JOULE_EXPANSION = 13,/**< The Intel Joule Expansion Board */
#if __STDC_VERSION__ >= 199901L
    MRAA_INTEL_GT_TUCHUCK = MRAA_INTEL_JOULE_EXPANSION, // deprecated
#endif
    MRAA_PHYBOARD_WEGA = 14,        /**< The phyBOARD-Wega */
    MRAA_DE_NANO_SOC = 15,          /**< Terasic DE-Nano-SoC Board */
    MRAA_UP2 = 16,                  /**< The UP^2 Board */
    MRAA_MTK_LINKIT = 17,           /**< Mediatek MT7688 based Linkit boards */
    MRAA_MTK_OMEGA2 = 18,           /**< MT7688 based Onion Omega2 board */
    MRAA_IEI_TANK = 19,             /**< IEI Tank System*/
    MRAA_ROCKPI4 = 20,              /**< Radxa ROCK PI 4 Models A/B/C */
    MRAA_ADLINK_IPI = 21,           /**< Adlink Industrial PI */
    MRAA_ADLINK_LEC_AL = 22,        /**< Adlink LEC-AL*/
    MRAA_ADLINK_LEC_AL_AI = 23,        /**< Adlink LEC-AL*/
    MRAA_UPXTREME = 24,             /**< The UPXTREME Board */
    MRAA_INTEL_ILK = 25,            /**< Intel Learning Kit */
    MRAA_SIEMENS_IOT2050 = 26,      /**< Siemens IOT2050 board */
    MRAA_RADXA_ROCK_3C = 27,        /**< Radxa ROCK 3 Model C */
    MRAA_VISIONFIVE = 28,           /**< StarFive VisionFive board */
    MRAA_RADXA_ROCK_5A = 29,        /**< Radxa ROCK 5 Model A */
    MRAA_RADXA_ROCK_5B = 30,        /**< Radxa ROCK 5 Model B */
    MRAA_ORANGE_PI_PRIME = 31,      /**< Orange Pi Prime board */
    MRAA_RADXA_ROCK_3B = 32,        /**< Radxa ROCK 3 Model B */
    MRAA_RADXA_CM3 = 33,            /**< Radxa CM3 */
    MRAA_RADXA_CM5_IO = 34,         /**< Radxa CM5 IO */
    MRAA_RADXA_ROCK_3A = 35,        /**< Radxa ROCK 3 Model A */
    MRAA_RADXA_E25 = 36,            /**< Radxa E25 */

    // USB platform extenders start at 256
    MRAA_FTDI_FT4222 = 256,         /**< FTDI FT4222 USB to i2c bridge */

    // contains bit 9 so is subplatform
    MRAA_GROVEPI = 1024,            /**< GrovePi shield i2c bridge */
    MRAA_GENERIC_FIRMATA = 1280,    /**< Firmata uart platform/bridge */

    MRAA_ANDROID_PERIPHERALMANAGER = 95, /**< Android Things peripheral manager platform */
    MRAA_MOCK_PLATFORM = 96,        /**< Mock platform, which requires no real hardware */
    MRAA_JSON_PLATFORM = 97,        /**< User initialised platform from json */
    MRAA_NULL_PLATFORM = 98,        /**< Platform with no capabilities that hosts a sub platform  */
    MRAA_UNKNOWN_PLATFORM = 99      /**< An unknown platform type, typically will load INTEL_GALILEO_GEN1 */
} mraa_platform_t;

/**
 * Intel edison miniboard numbering enum
 */
typedef enum {
    MRAA_INTEL_EDISON_MINIBOARD_J17_1 = 0,
    MRAA_INTEL_EDISON_MINIBOARD_J17_5 = 4,
    MRAA_INTEL_EDISON_MINIBOARD_J17_7 = 6,
    MRAA_INTEL_EDISON_MINIBOARD_J17_8 = 7,
    MRAA_INTEL_EDISON_MINIBOARD_J17_9 = 8,
    MRAA_INTEL_EDISON_MINIBOARD_J17_10 = 9,
    MRAA_INTEL_EDISON_MINIBOARD_J17_11 = 10,
    MRAA_INTEL_EDISON_MINIBOARD_J17_12 = 11,
    MRAA_INTEL_EDISON_MINIBOARD_J17_14 = 13,
    MRAA_INTEL_EDISON_MINIBOARD_J18_1 = 14,
    MRAA_INTEL_EDISON_MINIBOARD_J18_2 = 15,
    MRAA_INTEL_EDISON_MINIBOARD_J18_6 = 19,
    MRAA_INTEL_EDISON_MINIBOARD_J18_7 = 20,
    MRAA_INTEL_EDISON_MINIBOARD_J18_8 = 21,
    MRAA_INTEL_EDISON_MINIBOARD_J18_10 = 23,
    MRAA_INTEL_EDISON_MINIBOARD_J18_11 = 24,
    MRAA_INTEL_EDISON_MINIBOARD_J18_12 = 25,
    MRAA_INTEL_EDISON_MINIBOARD_J18_13 = 26,
    MRAA_INTEL_EDISON_MINIBOARD_J19_4 = 31,
    MRAA_INTEL_EDISON_MINIBOARD_J19_5 = 32,
    MRAA_INTEL_EDISON_MINIBOARD_J19_6 = 33,
    MRAA_INTEL_EDISON_MINIBOARD_J19_8 = 35,
    MRAA_INTEL_EDISON_MINIBOARD_J19_9 = 36,
    MRAA_INTEL_EDISON_MINIBOARD_J19_10 = 37,
    MRAA_INTEL_EDISON_MINIBOARD_J19_11 = 38,
    MRAA_INTEL_EDISON_MINIBOARD_J19_12 = 39,
    MRAA_INTEL_EDISON_MINIBOARD_J19_13 = 40,
    MRAA_INTEL_EDISON_MINIBOARD_J19_14 = 41,
    MRAA_INTEL_EDISON_MINIBOARD_J20_3 = 44,
    MRAA_INTEL_EDISON_MINIBOARD_J20_4 = 45,
    MRAA_INTEL_EDISON_MINIBOARD_J20_5 = 46,
    MRAA_INTEL_EDISON_MINIBOARD_J20_6 = 47,
    MRAA_INTEL_EDISON_MINIBOARD_J20_7 = 48,
    MRAA_INTEL_EDISON_MINIBOARD_J20_8 = 49,
    MRAA_INTEL_EDISON_MINIBOARD_J20_9 = 50,
    MRAA_INTEL_EDISON_MINIBOARD_J20_10 = 51,
    MRAA_INTEL_EDISON_MINIBOARD_J20_11 = 52,
    MRAA_INTEL_EDISON_MINIBOARD_J20_12 = 53,
    MRAA_INTEL_EDISON_MINIBOARD_J20_13 = 54,
    MRAA_INTEL_EDISON_MINIBOARD_J20_14 = 55
} mraa_intel_edison_miniboard_t;

/**
 * Intel Edison raw GPIO numbering enum
 */
typedef enum {
    MRAA_INTEL_EDISON_GP182 = 0,
    MRAA_INTEL_EDISON_GP135 = 4,
    MRAA_INTEL_EDISON_GP27 = 6,
    MRAA_INTEL_EDISON_GP20 = 7,
    MRAA_INTEL_EDISON_GP28 = 8,
    MRAA_INTEL_EDISON_GP111 = 0,
    MRAA_INTEL_EDISON_GP109 = 10,
    MRAA_INTEL_EDISON_GP115 = 11,
    MRAA_INTEL_EDISON_GP128 = 13,
    MRAA_INTEL_EDISON_GP13 = 14,
    MRAA_INTEL_EDISON_GP165 = 15,
    MRAA_INTEL_EDISON_GP19 = 19,
    MRAA_INTEL_EDISON_GP12 = 20,
    MRAA_INTEL_EDISON_GP183 = 21,
    MRAA_INTEL_EDISON_GP110 = 23,
    MRAA_INTEL_EDISON_GP114 = 24,
    MRAA_INTEL_EDISON_GP129 = 25,
    MRAA_INTEL_EDISON_GP130 = 26,
    MRAA_INTEL_EDISON_GP44 = 31,
    MRAA_INTEL_EDISON_GP46 = 32,
    MRAA_INTEL_EDISON_GP48 = 33,
    MRAA_INTEL_EDISON_GP131 = 35,
    MRAA_INTEL_EDISON_GP14 = 36,
    MRAA_INTEL_EDISON_GP40 = 37,
    MRAA_INTEL_EDISON_GP43 = 38,
    MRAA_INTEL_EDISON_GP77 = 39,
    MRAA_INTEL_EDISON_GP82 = 40,
    MRAA_INTEL_EDISON_GP83 = 41,
    MRAA_INTEL_EDISON_GP134 = 44,
    MRAA_INTEL_EDISON_GP45 = 45,
    MRAA_INTEL_EDISON_GP47 = 46,
    MRAA_INTEL_EDISON_GP49 = 47,
    MRAA_INTEL_EDISON_GP15 = 48,
    MRAA_INTEL_EDISON_GP84 = 49,
    MRAA_INTEL_EDISON_GP42 = 50,
    MRAA_INTEL_EDISON_GP41 = 51,
    MRAA_INTEL_EDISON_GP78 = 52,
    MRAA_INTEL_EDISON_GP79 = 53,
    MRAA_INTEL_EDISON_GP80 = 54,
    MRAA_INTEL_EDISON_GP81 = 55
} mraa_intel_edison_t;

/**
 * Radxa ROCK 5 Model A GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_ROCK_5A_PIN3 = 3,
    MRAA_RADXA_ROCK_5A_PIN5 = 5,
    MRAA_RADXA_ROCK_5A_PIN7 = 7,
    MRAA_RADXA_ROCK_5A_PIN8 = 8,
    MRAA_RADXA_ROCK_5A_PIN10 = 10,
    MRAA_RADXA_ROCK_5A_PIN11 = 11,
    MRAA_RADXA_ROCK_5A_PIN12 = 12,
    MRAA_RADXA_ROCK_5A_PIN13 = 13,
    MRAA_RADXA_ROCK_5A_PIN15 = 15,
    MRAA_RADXA_ROCK_5A_PIN16 = 16,
    MRAA_RADXA_ROCK_5A_PIN18 = 18,
    MRAA_RADXA_ROCK_5A_PIN19 = 19,
    MRAA_RADXA_ROCK_5A_PIN21 = 21,
    MRAA_RADXA_ROCK_5A_PIN22 = 22,
    MRAA_RADXA_ROCK_5A_PIN23 = 23,
    MRAA_RADXA_ROCK_5A_PIN24 = 24,
    MRAA_RADXA_ROCK_5A_PIN26 = 26,
    MRAA_RADXA_ROCK_5A_PIN27 = 27,
    MRAA_RADXA_ROCK_5A_PIN28 = 28,
    MRAA_RADXA_ROCK_5A_PIN29 = 29,
    MRAA_RADXA_ROCK_5A_PIN31 = 31,
    MRAA_RADXA_ROCK_5A_PIN32 = 32,
    MRAA_RADXA_ROCK_5A_PIN33 = 33,
    MRAA_RADXA_ROCK_5A_PIN35 = 35,
    MRAA_RADXA_ROCK_5A_PIN36 = 36,
    MRAA_RADXA_ROCK_5A_PIN37 = 37,
    MRAA_RADXA_ROCK_5A_PIN38 = 38,
    MRAA_RADXA_ROCK_5A_PIN40 = 40
} mraa_radxa_rock_5a_wiring_t;

/**
 * Radxa ROCK 5 Model B GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_ROCK_5B_PIN3 = 3,
    MRAA_RADXA_ROCK_5B_PIN5 = 5,
    MRAA_RADXA_ROCK_5B_PIN7 = 7,
    MRAA_RADXA_ROCK_5B_PIN8 = 8,
    MRAA_RADXA_ROCK_5B_PIN10 = 10,
    MRAA_RADXA_ROCK_5B_PIN11 = 11,
    MRAA_RADXA_ROCK_5B_PIN12 = 12,
    MRAA_RADXA_ROCK_5B_PIN13 = 13,
    MRAA_RADXA_ROCK_5B_PIN15 = 15,
    MRAA_RADXA_ROCK_5B_PIN16 = 16,
    MRAA_RADXA_ROCK_5B_PIN18 = 18,
    MRAA_RADXA_ROCK_5B_PIN19 = 19,
    MRAA_RADXA_ROCK_5B_PIN21 = 21,
    MRAA_RADXA_ROCK_5B_PIN22 = 22,
    MRAA_RADXA_ROCK_5B_PIN23 = 23,
    MRAA_RADXA_ROCK_5B_PIN24 = 24,
    MRAA_RADXA_ROCK_5B_PIN26 = 26,
    MRAA_RADXA_ROCK_5B_PIN27 = 27,
    MRAA_RADXA_ROCK_5B_PIN28 = 28,
    MRAA_RADXA_ROCK_5B_PIN29 = 29,
    MRAA_RADXA_ROCK_5B_PIN31 = 31,
    MRAA_RADXA_ROCK_5B_PIN32 = 32,
    MRAA_RADXA_ROCK_5B_PIN33 = 33,
    MRAA_RADXA_ROCK_5B_PIN35 = 35,
    MRAA_RADXA_ROCK_5B_PIN36 = 36,
    MRAA_RADXA_ROCK_5B_PIN37 = 37,
    MRAA_RADXA_ROCK_5B_PIN38 = 38,
    MRAA_RADXA_ROCK_5B_PIN40 = 40
} mraa_radxa_rock_5b_wiring_t;

/**
 * Radxa CM5 IO GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_CM5_IO_PIN3 = 3,
    MRAA_RADXA_CM5_IO_PIN5 = 5,
    MRAA_RADXA_CM5_IO_PIN7 = 7,
    MRAA_RADXA_CM5_IO_PIN8 = 8,
    MRAA_RADXA_CM5_IO_PIN10 = 10,
    MRAA_RADXA_CM5_IO_PIN11 = 11,
    MRAA_RADXA_CM5_IO_PIN12 = 12,
    MRAA_RADXA_CM5_IO_PIN13 = 13,
    MRAA_RADXA_CM5_IO_PIN15 = 15,
    MRAA_RADXA_CM5_IO_PIN16 = 16,
    MRAA_RADXA_CM5_IO_PIN18 = 18,
    MRAA_RADXA_CM5_IO_PIN19 = 19,
    MRAA_RADXA_CM5_IO_PIN21 = 21,
    MRAA_RADXA_CM5_IO_PIN22 = 22,
    MRAA_RADXA_CM5_IO_PIN23 = 23,
    MRAA_RADXA_CM5_IO_PIN24 = 24,
    MRAA_RADXA_CM5_IO_PIN26 = 26,
    MRAA_RADXA_CM5_IO_PIN29 = 29,
    MRAA_RADXA_CM5_IO_PIN31 = 31,
    MRAA_RADXA_CM5_IO_PIN32 = 32,
    MRAA_RADXA_CM5_IO_PIN33 = 33,
    MRAA_RADXA_CM5_IO_PIN35 = 35,
    MRAA_RADXA_CM5_IO_PIN36 = 36,
    MRAA_RADXA_CM5_IO_PIN38 = 38,
    MRAA_RADXA_CM5_IO_PIN40 = 40
} mraa_radxa_cm5_io_wiring_t;

/**
 *  Radxa ROCK 3 Model A GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_ROCK_3A_PIN3 = 3,
    MRAA_RADXA_ROCK_3A_PIN5 = 5,
    MRAA_RADXA_ROCK_3A_PIN7 = 7,
    MRAA_RADXA_ROCK_3A_PIN8 = 8,
    MRAA_RADXA_ROCK_3A_PIN10 = 10,
    MRAA_RADXA_ROCK_3A_PIN11 = 11,
    MRAA_RADXA_ROCK_3A_PIN12 = 12,
    MRAA_RADXA_ROCK_3A_PIN13 = 13,
    MRAA_RADXA_ROCK_3A_PIN15 = 15,
    MRAA_RADXA_ROCK_3A_PIN16 = 16,
    MRAA_RADXA_ROCK_3A_PIN17 = 17,
    MRAA_RADXA_ROCK_3A_PIN18 = 18,
    MRAA_RADXA_ROCK_3A_PIN19 = 19,
    MRAA_RADXA_ROCK_3A_PIN21 = 21,
    MRAA_RADXA_ROCK_3A_PIN23 = 23,
    MRAA_RADXA_ROCK_3A_PIN24 = 24,
    MRAA_RADXA_ROCK_3A_PIN26 = 26,
    MRAA_RADXA_ROCK_3A_PIN27 = 27,
    MRAA_RADXA_ROCK_3A_PIN28 = 28,
    MRAA_RADXA_ROCK_3A_PIN29 = 29,
    MRAA_RADXA_ROCK_3A_PIN31 = 31,
    MRAA_RADXA_ROCK_3A_PIN32 = 32,
    MRAA_RADXA_ROCK_3A_PIN33 = 33,
    MRAA_RADXA_ROCK_3A_PIN35 = 35,
    MRAA_RADXA_ROCK_3A_PIN36 = 36,
    MRAA_RADXA_ROCK_3A_PIN37 = 37,
    MRAA_RADXA_ROCK_3A_PIN38 = 38,
    MRAA_RADXA_ROCK_3A_PIN40 = 40
} mraa_radxa_rock_3a_wiring_t;

/**
 *  Radxa ROCK 3 Model B GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_ROCK_3B_PIN3 = 3,
    MRAA_RADXA_ROCK_3B_PIN5 = 5,
    MRAA_RADXA_ROCK_3B_PIN7 = 7,
    MRAA_RADXA_ROCK_3B_PIN8 = 8,
    MRAA_RADXA_ROCK_3B_PIN10 = 10,
    MRAA_RADXA_ROCK_3B_PIN11 = 11,
    MRAA_RADXA_ROCK_3B_PIN12 = 12,
    MRAA_RADXA_ROCK_3B_PIN13 = 13,
    MRAA_RADXA_ROCK_3B_PIN15 = 15,
    MRAA_RADXA_ROCK_3B_PIN16 = 16,
    MRAA_RADXA_ROCK_3B_PIN18 = 18,
    MRAA_RADXA_ROCK_3B_PIN19 = 19,
    MRAA_RADXA_ROCK_3B_PIN21 = 21,
    MRAA_RADXA_ROCK_3B_PIN22 = 22,
    MRAA_RADXA_ROCK_3B_PIN23 = 23,
    MRAA_RADXA_ROCK_3B_PIN24 = 24,
    MRAA_RADXA_ROCK_3B_PIN27 = 27,
    MRAA_RADXA_ROCK_3B_PIN28 = 28,
    MRAA_RADXA_ROCK_3B_PIN29 = 29,
    MRAA_RADXA_ROCK_3B_PIN31 = 31,
    MRAA_RADXA_ROCK_3B_PIN32 = 32,
    MRAA_RADXA_ROCK_3B_PIN33 = 33,
    MRAA_RADXA_ROCK_3B_PIN35 = 35,
    MRAA_RADXA_ROCK_3B_PIN36 = 36,
    MRAA_RADXA_ROCK_3B_PIN38 = 38,
    MRAA_RADXA_ROCK_3B_PIN40 = 40
} mraa_radxa_rock_3b_wiring_t;

/**
 *  Radxa ROCK 3 Model C GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_ROCK_3C_PIN3 = 3,
    MRAA_RADXA_ROCK_3C_PIN5 = 5,
    MRAA_RADXA_ROCK_3C_PIN7 = 7,
    MRAA_RADXA_ROCK_3C_PIN8 = 8,
    MRAA_RADXA_ROCK_3C_PIN10 = 10,
    MRAA_RADXA_ROCK_3C_PIN11 = 11,
    MRAA_RADXA_ROCK_3C_PIN12 = 12,
    MRAA_RADXA_ROCK_3C_PIN13 = 13,
    MRAA_RADXA_ROCK_3C_PIN15 = 15,
    MRAA_RADXA_ROCK_3C_PIN16 = 16,
    MRAA_RADXA_ROCK_3C_PIN18 = 18,
    MRAA_RADXA_ROCK_3C_PIN19 = 19,
    MRAA_RADXA_ROCK_3C_PIN21 = 21,
    MRAA_RADXA_ROCK_3C_PIN22 = 22,
    MRAA_RADXA_ROCK_3C_PIN23 = 23,
    MRAA_RADXA_ROCK_3C_PIN24 = 24,
    MRAA_RADXA_ROCK_3C_PIN27 = 27,
    MRAA_RADXA_ROCK_3C_PIN28 = 28,
    MRAA_RADXA_ROCK_3C_PIN29 = 29,
    MRAA_RADXA_ROCK_3C_PIN31 = 31,
    MRAA_RADXA_ROCK_3C_PIN32 = 32,
    MRAA_RADXA_ROCK_3C_PIN33 = 33,
    MRAA_RADXA_ROCK_3C_PIN35 = 35,
    MRAA_RADXA_ROCK_3C_PIN36 = 36,
    MRAA_RADXA_ROCK_3C_PIN37 = 37,
    MRAA_RADXA_ROCK_3C_PIN38 = 38,
    MRAA_RADXA_ROCK_3C_PIN40 = 40
} mraa_radxa_rock_3c_wiring_t;

/**
 *  Radxa CM3 IO GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_CM3_IO_PIN3 = 3,
    MRAA_RADXA_CM3_IO_PIN5 = 5,
    MRAA_RADXA_CM3_IO_PIN7 = 7,
    MRAA_RADXA_CM3_IO_PIN8 = 8,
    MRAA_RADXA_CM3_IO_PIN10 = 10,
    MRAA_RADXA_CM3_IO_PIN11 = 11,
    MRAA_RADXA_CM3_IO_PIN12 = 12,
    MRAA_RADXA_CM3_IO_PIN13 = 13,
    MRAA_RADXA_CM3_IO_PIN15 = 15,
    MRAA_RADXA_CM3_IO_PIN16 = 16,
    MRAA_RADXA_CM3_IO_PIN18 = 18,
    MRAA_RADXA_CM3_IO_PIN19 = 19,
    MRAA_RADXA_CM3_IO_PIN21 = 21,
    MRAA_RADXA_CM3_IO_PIN22 = 22,
    MRAA_RADXA_CM3_IO_PIN23 = 23,
    MRAA_RADXA_CM3_IO_PIN24 = 24,
    MRAA_RADXA_CM3_IO_PIN27 = 27,
    MRAA_RADXA_CM3_IO_PIN28 = 28,
    MRAA_RADXA_CM3_IO_PIN29 = 29,
    MRAA_RADXA_CM3_IO_PIN31 = 31,
    MRAA_RADXA_CM3_IO_PIN32 = 32,
    MRAA_RADXA_CM3_IO_PIN33 = 33,
    MRAA_RADXA_CM3_IO_PIN35 = 35,
    MRAA_RADXA_CM3_IO_PIN36 = 36,
    MRAA_RADXA_CM3_IO_PIN37 = 37,
    MRAA_RADXA_CM3_IO_PIN38 = 38,
    MRAA_RADXA_CM3_IO_PIN40 = 40
} mraa_radxa_cm3_io_wiring_t;

/**
 *  Radxa E25 GPIO numbering enum
 */
typedef enum {
    MRAA_RADXA_E25_PIN3 = 3,
    MRAA_RADXA_E25_PIN5 = 5,
    MRAA_RADXA_E25_PIN7 = 7,
    MRAA_RADXA_E25_PIN8 = 8,
    MRAA_RADXA_E25_PIN10 = 10,
    MRAA_RADXA_E25_PIN11 = 11,
    MRAA_RADXA_E25_PIN12 = 12,
    MRAA_RADXA_E25_PIN13 = 13,
    MRAA_RADXA_E25_PIN15 = 15,
    MRAA_RADXA_E25_PIN16 = 16,
    MRAA_RADXA_E25_PIN17 = 17,
    MRAA_RADXA_E25_PIN18 = 18,
    MRAA_RADXA_E25_PIN19 = 19,
    MRAA_RADXA_E25_PIN21 = 21,
    MRAA_RADXA_E25_PIN22 = 22,
    MRAA_RADXA_E25_PIN23 = 23,
    MRAA_RADXA_E25_PIN24 = 24,
    MRAA_RADXA_E25_PIN26 = 26
} mraa_radxa_e25_wiring_t;

/**
 *  ROCKPI4 GPIO numbering enum
 */
typedef enum {
    MRAA_ROCKPI4_PIN3 = 3,
    MRAA_ROCKPI4_PIN5 = 5,
    MRAA_ROCKPI4_PIN7 = 7,
    MRAA_ROCKPI4_PIN8 = 8,
    MRAA_ROCKPI4_PIN10 = 10,
    MRAA_ROCKPI4_PIN11 = 11,
    MRAA_ROCKPI4_PIN12 = 12,
    MRAA_ROCKPI4_PIN13 = 13,
    MRAA_ROCKPI4_PIN15 = 15,
    MRAA_ROCKPI4_PIN16 = 16,
    MRAA_ROCKPI4_PIN18 = 18,
    MRAA_ROCKPI4_PIN19 = 19,
    MRAA_ROCKPI4_PIN21 = 21,
    MRAA_ROCKPI4_PIN22 = 22,
    MRAA_ROCKPI4_PIN23 = 23,
    MRAA_ROCKPI4_PIN24 = 24,
    MRAA_ROCKPI4_PIN27 = 27,
    MRAA_ROCKPI4_PIN28 = 28,
    MRAA_ROCKPI4_PIN29 = 29,
    MRAA_ROCKPI4_PIN31 = 31,
    MRAA_ROCKPI4_PIN32 = 32,
    MRAA_ROCKPI4_PIN33 = 33,
    MRAA_ROCKPI4_PIN35 = 35,
    MRAA_ROCKPI4_PIN36 = 36,
    MRAA_ROCKPI4_PIN37 = 37,
    MRAA_ROCKPI4_PIN38 = 38,
    MRAA_ROCKPI4_PIN40 = 40
} mraa_rockpi4_wiring_t;

/**
* Raspberry PI Wiring compatible numbering enum
*/
typedef enum {
    MRAA_RASPBERRY_WIRING_PIN8 = 3,
    MRAA_RASPBERRY_WIRING_PIN9 = 5,
    MRAA_RASPBERRY_WIRING_PIN7 = 7,
    MRAA_RASPBERRY_WIRING_PIN15 = 8,
    MRAA_RASPBERRY_WIRING_PIN16 = 10,
    MRAA_RASPBERRY_WIRING_PIN0 = 11,
    MRAA_RASPBERRY_WIRING_PIN1 = 12,
    MRAA_RASPBERRY_WIRING_PIN2 = 13,
    MRAA_RASPBERRY_WIRING_PIN3 = 15,
    MRAA_RASPBERRY_WIRING_PIN4 = 16,
    MRAA_RASPBERRY_WIRING_PIN5 = 18,
    MRAA_RASPBERRY_WIRING_PIN12 = 19,
    MRAA_RASPBERRY_WIRING_PIN13 = 21,
    MRAA_RASPBERRY_WIRING_PIN6 = 22,
    MRAA_RASPBERRY_WIRING_PIN14 = 23,
    MRAA_RASPBERRY_WIRING_PIN10 = 24,
    MRAA_RASPBERRY_WIRING_PIN11 = 26,
    MRAA_RASPBERRY_WIRING_PIN17 = 29, // RPi B V2
    MRAA_RASPBERRY_WIRING_PIN21 = 29,
    MRAA_RASPBERRY_WIRING_PIN18 = 30, // RPi B V2
    MRAA_RASPBERRY_WIRING_PIN19 = 31, // RPI B V2
    MRAA_RASPBERRY_WIRING_PIN22 = 31,
    MRAA_RASPBERRY_WIRING_PIN20 = 32, // RPi B V2
    MRAA_RASPBERRY_WIRING_PIN26 = 32,
    MRAA_RASPBERRY_WIRING_PIN23 = 33,
    MRAA_RASPBERRY_WIRING_PIN24 = 35,
    MRAA_RASPBERRY_WIRING_PIN27 = 36,
    MRAA_RASPBERRY_WIRING_PIN25 = 37,
    MRAA_RASPBERRY_WIRING_PIN28 = 38,
    MRAA_RASPBERRY_WIRING_PIN29 = 40
} mraa_raspberry_wiring_t;

/**
 *  Orange Pi Prime GPIO numbering enum
 */
typedef enum {
    MRAA_ORANGE_PI_PRIME_PIN3 = 3,
    MRAA_ORANGE_PI_PRIME_PIN5 = 5,
    MRAA_ORANGE_PI_PRIME_PIN7 = 7,
    MRAA_ORANGE_PI_PRIME_PIN8 = 8,
    MRAA_ORANGE_PI_PRIME_PIN10 = 10,
    MRAA_ORANGE_PI_PRIME_PIN11 = 11,
    MRAA_ORANGE_PI_PRIME_PIN12 = 12,
    MRAA_ORANGE_PI_PRIME_PIN13 = 13,
    MRAA_ORANGE_PI_PRIME_PIN15 = 15,
    MRAA_ORANGE_PI_PRIME_PIN16 = 16,
    MRAA_ORANGE_PI_PRIME_PIN18 = 18,
    MRAA_ORANGE_PI_PRIME_PIN19 = 19,
    MRAA_ORANGE_PI_PRIME_PIN21 = 21,
    MRAA_ORANGE_PI_PRIME_PIN22 = 22,
    MRAA_ORANGE_PI_PRIME_PIN23 = 23,
    MRAA_ORANGE_PI_PRIME_PIN24 = 24,
    MRAA_ORANGE_PI_PRIME_PIN26 = 26,
    MRAA_ORANGE_PI_PRIME_PIN27 = 27,
    MRAA_ORANGE_PI_PRIME_PIN28 = 28,
    MRAA_ORANGE_PI_PRIME_PIN29 = 29,
    MRAA_ORANGE_PI_PRIME_PIN31 = 31,
    MRAA_ORANGE_PI_PRIME_PIN32 = 32,
    MRAA_ORANGE_PI_PRIME_PIN33 = 33,
    MRAA_ORANGE_PI_PRIME_PIN35 = 35,
    MRAA_ORANGE_PI_PRIME_PIN36 = 36,
    MRAA_ORANGE_PI_PRIME_PIN37 = 37,
    MRAA_ORANGE_PI_PRIME_PIN38 = 38,
    MRAA_ORANGE_PI_PRIME_PIN40 = 40
} mraa_orange_pi_prime_wiring_t;

/**
 * MRAA return codes
 */
typedef enum {
    MRAA_SUCCESS = 0,                             /**< Expected response */
    MRAA_ERROR_FEATURE_NOT_IMPLEMENTED = 1,       /**< Feature TODO */
    MRAA_ERROR_FEATURE_NOT_SUPPORTED = 2,         /**< Feature not supported by HW */
    MRAA_ERROR_INVALID_VERBOSITY_LEVEL = 3,       /**< Verbosity level wrong */
    MRAA_ERROR_INVALID_PARAMETER = 4,             /**< Parameter invalid */
    MRAA_ERROR_INVALID_HANDLE = 5,                /**< Handle invalid */
    MRAA_ERROR_NO_RESOURCES = 6,                  /**< No resource of that type avail */
    MRAA_ERROR_INVALID_RESOURCE = 7,              /**< Resource invalid */
    MRAA_ERROR_INVALID_QUEUE_TYPE = 8,            /**< Queue type incorrect */
    MRAA_ERROR_NO_DATA_AVAILABLE = 9,             /**< No data available */
    MRAA_ERROR_INVALID_PLATFORM = 10,             /**< Platform not recognised */
    MRAA_ERROR_PLATFORM_NOT_INITIALISED = 11,     /**< Board information not initialised */
    MRAA_ERROR_UART_OW_SHORTED = 12,              /**< UART OW Short Circuit Detected*/
    MRAA_ERROR_UART_OW_NO_DEVICES = 13,           /**< UART OW No devices detected */
    MRAA_ERROR_UART_OW_DATA_ERROR = 14,           /**< UART OW Data/Bus error detected */

    MRAA_ERROR_UNSPECIFIED = 99 /**< Unknown Error */
} mraa_result_t;

/**
 * Enum representing different possible modes for a pin.
 */
typedef enum {
    MRAA_PIN_VALID = 0,     /**< Pin Valid */
    MRAA_PIN_GPIO = 1,      /**< General Purpose IO */
    MRAA_PIN_PWM = 2,       /**< Pulse Width Modulation */
    MRAA_PIN_FAST_GPIO = 3, /**< Faster GPIO */
    MRAA_PIN_SPI = 4,       /**< SPI */
    MRAA_PIN_I2C = 5,       /**< I2C */
    MRAA_PIN_AIO = 6,       /**< Analog in */
    MRAA_PIN_UART = 7       /**< UART */
} mraa_pinmodes_t;

/**
 * Enum reprensenting different i2c speeds/modes
 */
typedef enum {
    MRAA_I2C_STD = 0,  /**< up to 100Khz */
    MRAA_I2C_FAST = 1, /**< up to 400Khz */
    MRAA_I2C_HIGH = 2  /**< up to 3.4Mhz */
} mraa_i2c_mode_t;

/**
 * Enum representing different uart parity states
 */
typedef enum {
    MRAA_UART_PARITY_NONE = 0,
    MRAA_UART_PARITY_EVEN = 1,
    MRAA_UART_PARITY_ODD = 2,
    MRAA_UART_PARITY_MARK = 3,
    MRAA_UART_PARITY_SPACE = 4
} mraa_uart_parity_t;

#ifdef __cplusplus
}
#endif
