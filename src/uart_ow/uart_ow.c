/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2015 Intel Corporation
 *
 * This code is based on the onewire-over-uart git repository at:
 * https://github.com/dword1511/onewire-over-uart with various authors
 * and contributors listed as:
 *
 * Colin O'Flynn, Martin Thomas, Chi Zhang, and Peter Dannegger
 *
 * It's theory of operation is described here:
 * https://www.maximintegrated.com/en/app-notes/index.mvp/id/214
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

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include "uart.h"
#include "uart_ow.h"
#include "mraa_internal.h"

static unsigned char
read_byte(mraa_uart_ow_context dev)
{
  static unsigned char ch = 0;

  if (!dev) {
    syslog(LOG_ERR, "uart_ow: read: context is NULL");
    return MRAA_ERROR_INVALID_HANDLE;
  }
  
  if (dev->fd < 0) {
    syslog(LOG_ERR, "uart_ow: port is not open");
        return MRAA_ERROR_INVALID_RESOURCE;
  }
  
  while (!read(dev->fd, &ch, 1))
    ;

  return ch;
}

static void
write_byte(mraa_uart_ow_context dev, const unsigned char ch)
{
  if (!dev) {
    syslog(LOG_ERR, "uart_ow: write: context is NULL");
    return;
  }
  
  if (dev->fd < 0) {
    syslog(LOG_ERR, "uart_ow: port is not open");
    return;
  }
  
  write(dev->fd, &ch, 1);
}

static void
set_speed(mraa_uart_context dev, mraa_boolean_t speed)
{
  static speed_t baud = B9600;

  if (!dev) {
    syslog(LOG_ERR, "uart_ow: set_speed: context is NULL");
    return;
  }

  if (speed)
    baud = B115200;
  else
    baud = B9600;

  // these settings are required, taken from onewire-over-uart code.
  struct termios termio = { 
    .c_cflag = baud | CS8 | CLOCAL | CREAD,
    .c_iflag = 0,
    .c_oflag = 0,
    .c_lflag = NOFLSH,
    .c_cc = {0},
  };

  tcflush(dev->fd, TCIFLUSH);

  // TCSANOW is required
  if (tcsetattr(dev->fd, TCSANOW, &termio) < 0) {
    syslog(LOG_ERR, "uart_ow: tcsetattr() failed");
    return;
  }

  return;
}

mraa_uart_ow_context
mraa_uart_ow_init(int index)
{
  mraa_uart_ow_context dev = mraa_uart_init(index);

  if (!dev)
    return NULL;

  // now get the fd, and set it up for non-blocking operation
  if (fcntl(dev->fd, F_SETFL, O_NONBLOCK) == -1)
    {
      syslog(LOG_ERR, "uart_ow: failed to set non-blocking on fd");
      mraa_uart_ow_stop(dev);
      return NULL;
    }

  return dev;
}

mraa_uart_ow_context
mraa_uart_ow_init_raw(const char* path)
{
  mraa_uart_ow_context dev = mraa_uart_init_raw(path);

  if (!dev)
    return NULL;

  // now get the fd, and set it up for non-blocking operation
  if (fcntl(dev->fd, F_SETFL, O_NONBLOCK) == -1)
    {
      syslog(LOG_ERR, "uart_ow: failed to set non-blocking on fd");
      mraa_uart_ow_stop(dev);
      return NULL;
    }

  return dev;
}

mraa_result_t
mraa_uart_ow_stop(mraa_uart_ow_context dev)
{
  return mraa_uart_stop(dev);
}

const char*
mraa_uart_ow_get_dev_path(mraa_uart_ow_context dev)
{
  return mraa_uart_get_dev_path(dev);
}

uint8_t
mraa_uart_ow_bit(mraa_uart_ow_context dev, uint8_t bit)
{
  if (!dev) {
    syslog(LOG_ERR, "uart_ow: ow_bit: context is NULL");
    return 0;
  }

  if (bit)
    write_byte(dev, 0xff);      /* write a 1 bit */
  else
    write_byte(dev, 0x00);
    
  return (read_byte(dev) == 0xff);
}

uint8_t
mraa_uart_ow_write_byte(mraa_uart_ow_context dev, uint8_t byte)
{
  if (!dev) {
    syslog(LOG_ERR, "uart_ow: write_byte: context is NULL");
    return 0;
  }

  uint8_t i = 8;
  uint8_t b;

  do 
    {
      b = mraa_uart_ow_bit(dev, byte & 0x01);
      byte >>= 1;
      if (b)                    /* we're reading while we write */
        byte |= 0x80;
    } while (--i);

  return byte;
}

uint8_t
mraa_uart_ow_read_byte(mraa_uart_ow_context dev)
{
  if (!dev) {
    syslog(LOG_ERR, "uart_ow: read_byte: context is NULL");
    return 0;
  }

  /* we read by sending ones, so the bus is released on the init low
   * pulse (start bit) for every slot, when the device can then send a
   * bit
   */
  return mraa_uart_ow_write_byte(dev, 0xff);
}

mraa_uart_ow_status_t
mraa_uart_ow_reset(mraa_uart_ow_context dev)
{
  if (!dev) {
    syslog(LOG_ERR, "uart_ow: reset: context is NULL");
    return MRAA_UART_OW_DATA_ERROR;
  }

  uint8_t rv;

  /* we set low speed (9600 baud) for the reset pulse, then switch
   * back to high speed (115200 baud)
   */
  set_speed(dev, 0);
  /* pull the data line low */
  write_byte(dev, 0xf0);
  rv = read_byte(dev);
  set_speed(dev, 1);

  /* shorted data line */
  if (rv == 0x00)
    return MRAA_UART_OW_SHORTED;
  /* no devices detected (no presence pulse) */
  if (rv == 0xf0)
    return MRAA_UART_OW_NO_DEVICES;
    
  return MRAA_UART_OW_SUCCESS;
}

uint8_t 
mraa_uart_ow_rom_search(mraa_uart_ow_context dev, uint8_t start, uint8_t *id)
{
  if (!dev) {
    syslog(LOG_ERR, "uart_ow: rom_search: context is NULL");
    return MRAA_UART_OW_DATA_ERROR;
  }

  uint8_t i, j, next_start;
  uint8_t b;

  /* error, no device found <--- early exit! */
  if(mraa_uart_ow_reset(dev) != MRAA_UART_OW_SUCCESS)
    return MRAA_UART_OW_NO_DEVICES;

  mraa_uart_ow_write_byte(dev, MRAA_UART_OW_CMD_SEARCH_ROM);
  /* unchanged on the last device */
  next_start = MRAA_UART_OW_SEARCH_ROM_LAST;

  i = MRAA_UART_OW_ROMCODE_SIZE * 8; /* 8 bytes */

  do 
    {
      j = 8; /* 8 bits */
      do 
        {
          b = mraa_uart_ow_bit(dev, 1); /* read bit */
          if (mraa_uart_ow_bit(dev, 1)) /* read complement */
            {
              /* 0b11: data error/no devices */
              if (b)
                return MRAA_UART_OW_DATA_ERROR;
            }
          else
            {
              if (!b)
                {
                  if (start > i || ((*id & 1) && start != i))
                    { /* 0b00 = 2 devices (a discrepancy) */
                      b = 1; /* now 1 */
                      next_start = i; /* next pass 0 */
                    }
                }
            }
          
          mraa_uart_ow_bit(dev, b); /* write bit */
          *id >>= 1;
          if (b)
            *id |= 0x80; /* store bit */
          
          i--;
        } while (--j);

      id++; /* next byte */
    } while (i);

  return next_start; /* to continue search */
}

void 
mraa_uart_ow_command(mraa_uart_ow_context dev, uint8_t command, uint8_t *id)
{
  if (!dev) {
    syslog(LOG_ERR, "uart_ow: ow_command: context is NULL");
    return;
  }

  uint8_t i;

  mraa_uart_ow_reset(dev);
  
  if (id) {
    /* sending to a specific device */
    mraa_uart_ow_write_byte(dev, MRAA_UART_OW_CMD_MATCH_ROM);
    i = MRAA_UART_OW_ROMCODE_SIZE;
    do {
      mraa_uart_ow_write_byte(dev, *id);
      id++;
    } while(--i);
  }
  else
    {
      /* send to all devices */
      mraa_uart_ow_write_byte(dev, MRAA_UART_OW_CMD_SKIP_ROM);
    }

  mraa_uart_ow_write_byte(dev, command);
}

uint8_t
mraa_uart_ow_crc8(uint8_t *buffer, uint16_t length)
{
  static const uint8_t CRC8POLY = 0x18; // 0x18 = X ^ 8 + X ^ 5 + X ^ 4 + X ^ 0

  uint8_t crc = 0x00;
  uint16_t loop_count;
  uint8_t bit_counter;
  uint8_t data;
  uint8_t feedback_bit;

  for (loop_count = 0; loop_count != length; loop_count++) {
    data = buffer[loop_count];
    bit_counter = 8;
    do {
      feedback_bit = (crc ^ data) & 0x01;
      if (feedback_bit == 0x01)
        crc = crc ^ CRC8POLY;
      crc = (crc >> 1) & 0x7F;
      if (feedback_bit == 0x01)
        crc = crc | 0x80;
      data = data >> 1;
      bit_counter--;
    } while (bit_counter > 0);
  }

  return crc;
}

