// Library:   libi2c API
// File:      smbus.c

/*! \file
 *
 * $LastChangedDate: 2009-09-09 09:44:12 -0600 (Wed, 09 Sep 2009) $
 * $Rev: 130 $
 *
 * \brief System Management Bus (SMBus) over I<sup>2</sup>C communication
 * interface.
 *
 * \author Robin Knight (robin.knight@roadnarrows.com)
 *
 * \par Copyright:
 * (C) 2009.  RoadNarrows LLC.
 * (http://www.roadnarrows.com) \n
 * All Rights Reserved
 *
 * <hr>
 * \par Original Source and Copyright:
 * See i2c-dev.h.
 *
 * <hr>
 */
// Permission is hereby granted, without written agreement and without
// license or royalty fees, to use, copy, modify, and distribute this
// software and its documentation for any purpose, provided that
// (1) The above copyright notice and the following two paragraphs
// appear in all copies of the source code and (2) redistributions
// including binaries reproduces these notices in the supporting
// documentation.   Substantial modifications to this software may be
// copyrighted by their authors and need not follow the licensing terms
// described here, provided that the new terms are clearly indicated in
// all files where they apply.
//
// IN NO EVENT SHALL THE AUTHOR, ROADNARROWS LLC, OR ANY MEMBERS/EMPLOYEES
// OF ROADNARROW LLC OR DISTRIBUTORS OF THIS SOFTWARE BE LIABLE TO ANY
// PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE AUTHORS OR ANY OF THE ABOVE PARTIES HAVE BEEN ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
// THE AUTHOR AND ROADNARROWS LLC SPECIFICALLY DISCLAIM ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN
// "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO
// PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
////////////////////////////////////////////////////////////////////////////////

#include "smbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Execute an SMBus IOCTL.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param read_write    Operation access type.
 * \param command       Operation command or immediate data.
 * \param size          Data size.
 * \param [in,out] data Read/write/control data
 *
 * \return
 *  Returns \h_ge 0 on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_access(int fd, uint8_t read_write, uint8_t command,
                      int size, i2c_smbus_data_t *data)
{
  i2c_smbus_ioctl_data_t args;

  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;

  return ioctl(fd, I2C_SMBUS, &args);
}

/*!
 * \brief Write a quick value to the SMBus.
 *
 * \param fd        File descriptor to opened SMBus device.
 * \param value     Value to write
 *
 * \return
 *  Returns \h_ge 0 on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_write_quick(int fd, uint8_t value)
{
  return i2c_smbus_access(fd, value, I2C_NOCMD, I2C_SMBUS_QUICK, NULL);
}

/*!
 * \brief Read an immediate byte from the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 *
 * \return
 *  Returns read byte on on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_read_byte(int fd)
{
  i2c_smbus_data_t  data;
  int               rc;

  rc = i2c_smbus_access(fd, I2C_SMBUS_READ, I2C_NOCMD, I2C_SMBUS_BYTE, &data);

  return rc>=0? 0x0FF & data.byte: -1;
}

/*!
 * \brief Write an immediate byte to the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param value         Byte value to write.
 *
 * \return
 *  Returns \h_ge 0 on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_write_byte(int fd, uint8_t value)
{
  return i2c_smbus_access(fd, I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE, NULL);
}

/*!
 * \brief Read a data byte from the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 *
 * \return
 *  Returns read byte on on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_read_byte_data(int fd, uint8_t command)
{
  i2c_smbus_data_t  data;
  int               rc;

  rc = i2c_smbus_access(fd, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA,
                              &data);

  return rc>=0? 0x0FF & data.byte: -1;
}

/*!
 * \brief Write a data byte to the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 * \param value         Byte value to write.
 *
 * \return
 *  Returns \h_ge 0 on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_write_byte_data(int fd, uint8_t command, uint8_t value)
{
  i2c_smbus_data_t  data;

  data.byte = value;

  return i2c_smbus_access(fd, I2C_SMBUS_WRITE, command,
                          I2C_SMBUS_BYTE_DATA, &data);
}

/*!
 * \brief Read a data 2-byte word from the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 *
 * \return
 *  Returns read 2-byte word on on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_read_word_data(int fd, uint8_t command)
{
  i2c_smbus_data_t  data;
  int               rc;

  rc = i2c_smbus_access(fd, I2C_SMBUS_READ, command, I2C_SMBUS_WORD_DATA,
                          &data);

  return rc>=0? 0x0FFFF & data.word: -1;
}

/*!
 * \brief Write a data 2-byte word to the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 * \param value         Word value to write.
 *
 * \return
 *  Returns \h_ge 0 on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_write_word_data(int fd, uint8_t command, unsigned short value)
{
  i2c_smbus_data_t  data;

  data.word = value;

  return i2c_smbus_access(fd, I2C_SMBUS_WRITE,  command,
                          I2C_SMBUS_WORD_DATA, &data);
}

/*!
 * \brief Issue a 2-byte word process call (write/read) to the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 * \param value         Word value to write.
 *
 * \return
 *  Returns read 2-byte word on on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_process_call(int fd, uint8_t command, unsigned short value)
{
  i2c_smbus_data_t  data;
  int               rc;

  data.word = value;

  rc = i2c_smbus_access(fd, I2C_SMBUS_WRITE, command, I2C_SMBUS_PROC_CALL,
                        &data);

  return rc>=0? 0x0FFFF & data.word: -1;
}


/*!
 * \brief Read a block of data from the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 * \param [out] values  Buffer to hold the block of read byte values.\n
 *                      Must be large enough to receive the data.
 *
 * \return
 *  On success, returns \h_ge 0 the number of bytes read, excluding any header
 *  fields. Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_read_block_data(int fd, uint8_t command, uint8_t *values)
{
  i2c_smbus_data_t  data;
  int               i;
  int               rc;

  rc = i2c_smbus_access(fd, I2C_SMBUS_READ, command, I2C_SMBUS_BLOCK_DATA,
                          &data);

  if( rc >= 0 )
  {
    for(i=1; i<=data.block[0]; ++i)
    {
      values[i-1] = data.block[i];
    }
    rc = data.block[0];
  }

  return rc;
}

/*!
 * \brief Write a data block to the SMBus.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 * \param length        Length of buffer (bytes) to write.
 * \param [in] values   Buffer of data to write.
 *
 * \return
 *  Returns \h_ge 0 on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_write_block_data(int fd, uint8_t command, uint8_t length,
                              const uint8_t *values)
{
  i2c_smbus_data_t  data;
  int               i;

  if( length > I2C_SMBUS_BLOCK_MAX )
  {
    length = I2C_SMBUS_BLOCK_MAX;
  }

  for(i=1; i<=length; i++)
  {
    data.block[i] = values[i-1];
  }
  data.block[0] = length;

  return i2c_smbus_access(fd, I2C_SMBUS_WRITE, command,
                          I2C_SMBUS_BLOCK_DATA, &data);
}

/*!
 * \brief Read a block of data from the SMBus via low-level I<sup>2</sup>C.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 * \param [out] values  Buffer to hold the block of read byte values.\n
 *                      Must be large enough to receive the data.
 *
 * \return
 *  On success, returns \h_ge 0 the number of bytes read, excluding any header
 *  fields. Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_read_i2c_block_data(int fd, uint8_t command, uint8_t *values)
{
  i2c_smbus_data_t  data;
  int               i;
  int               rc;

  rc = i2c_smbus_access(fd, I2C_SMBUS_READ, command, I2C_SMBUS_I2C_BLOCK_DATA,
                        &data);
  if( rc >= 0 )
  {
    for(i=1; i<=data.block[0]; i++)
    {
      values[i-1] = data.block[i];
    }
    rc = data.block[0];
  }
  return rc;
}

/*!
 * \brief Write a block of data to the SMBus via low-level I<sup>2</sup>C.
 *
 * \param fd            File descriptor to opened SMBus device.
 * \param command       Command to SMBus device.
 * \param length        Length of buffer (bytes) to write.
 * \param [in] values   Buffer of data to write.
 *
 * \return
 *  Returns \h_ge 0 on success.
 *  Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_write_i2c_block_data(int fd, uint8_t command, uint8_t length,
                                  const uint8_t *values)
{
  i2c_smbus_data_t  data;
  int               i;

  if( length > I2C_SMBUS_I2C_BLOCK_MAX )
  {
    length = I2C_SMBUS_I2C_BLOCK_MAX;
  }

  for(i=1; i<=length; i++)
  {
    data.block[i] = values[i-1];
  }
  data.block[0] = length;

  return i2c_smbus_access(fd, I2C_SMBUS_WRITE, command,
                          I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

/*!
 * \brief Issue a block process call (write/read) to the SMBus.
 *
 * \param fd                File descriptor to opened SMBus device.
 * \param command           Command to SMBus device.
 * \param length            Length of buffer (bytes) to write.
 * \param [in,out] values   Buffer of data to write and to hold the block of
 *                          read byte values.\n
 *                          Must be large enough to receive the data.
 *
 * \return
 *  On success, returns \h_ge 0 the number of bytes read, excluding any header
 *  fields. Else errno is set appropriately and -1 is returned.
 */
int i2c_smbus_block_process_call(int fd, uint8_t command, uint8_t length,
                                 uint8_t *values)
{
  i2c_smbus_data_t  data;
  int               i;
  int               rc;

  if( length > I2C_SMBUS_BLOCK_MAX )
  {
    length = I2C_SMBUS_BLOCK_MAX;
  }

  for(i=1; i<=length; i++)
  {
    data.block[i] = values[i-1];
  }
  data.block[0] = length;

  rc = i2c_smbus_access(fd, I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_PROC_CALL,
                          &data);

  if( rc >= 0 )
  {
    for(i=1; i<=data.block[0]; i++)
    {
      values[i-1] = data.block[i];
    }
    rc = data.block[0];
  }

  return rc;
}

#ifdef __cplusplus
}
#endif
