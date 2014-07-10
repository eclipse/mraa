/*
 * Author: Robin Knight (robin.knight@roadnarrows.com)
 *
 * Copyright © 2009 RoadNarrows LLC.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that
 * (1) The above copyright notice and the following two paragraphs
 * appear in all copies of the source code and (2) redistributions
 * including binaries reproduces these notices in the supporting
 * documentation.   Substantial modifications to this software may be
 * copyrighted by their authors and need not follow the licensing terms
 * described here, provided that the new terms are clearly indicated in
 * all files where they apply.
 *
 * IN NO EVENT SHALL THE AUTHOR, ROADNARROWS LLC, OR ANY MEMBERS/EMPLOYEES
 * OF ROADNARROW LLC OR DISTRIBUTORS OF THIS SOFTWARE BE LIABLE TO ANY
 * PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE AUTHORS OR ANY OF THE ABOVE PARTIES HAVE BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR AND ROADNARROWS LLC SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN
 * "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ioctl.h>

#include "linux/i2c-dev.h"

typedef union i2c_smbus_data_union
{
    uint8_t byte; ///< data byte
    unsigned short word; ///< data short word
    uint8_t block[I2C_SMBUS_BLOCK_MAX + 2];
    ///< block[0] is used for length and one more for PEC
} i2c_smbus_data_t;

typedef struct i2c_smbus_ioctl_data_struct
{
    uint8_t read_write; ///< operation direction
    uint8_t command; ///< ioctl command
    int size; ///< data size
    i2c_smbus_data_t *data; ///< data
} i2c_smbus_ioctl_data_t;

// ---------------------------------------------------------------------------
// Prototypes
// ---------------------------------------------------------------------------

extern int i2c_smbus_access(int fd, uint8_t read_write, uint8_t command,
                      int size, i2c_smbus_data_t *data);

extern int i2c_smbus_write_quick(int fd, uint8_t value);

extern int i2c_smbus_read_byte(int fd);

extern int i2c_smbus_write_byte(int fd, uint8_t value);

extern int i2c_smbus_read_byte_data(int fd, uint8_t command);

extern int i2c_smbus_write_byte_data(int fd, uint8_t command, uint8_t value);

extern int i2c_smbus_read_word_data(int fd, uint8_t command);

extern int i2c_smbus_write_word_data(int fd, uint8_t command, unsigned short value);

extern int i2c_smbus_process_call(int fd, uint8_t command, unsigned short value);

extern int i2c_smbus_read_block_data(int fd, uint8_t command, uint8_t *values);

extern int i2c_smbus_write_block_data(int fd, uint8_t command, uint8_t length,
                                        const uint8_t *values);
extern int i2c_smbus_read_i2c_block_data(int fd, uint8_t command,
                                          uint8_t *values);

extern int i2c_smbus_write_i2c_block_data(int fd, uint8_t command, uint8_t length,
                                    const uint8_t *values);

extern int i2c_smbus_block_process_call(int fd, uint8_t command, uint8_t length,
                                          uint8_t *values);

#ifdef __cplusplus
}
#endif
