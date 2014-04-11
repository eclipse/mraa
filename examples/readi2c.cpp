#include "maa.h"

int
main ()
{
  maa::I2CSlave i2c(26, 27);

  int addr = 0x62;
  i2c.address(addr);

  char data[2];
  int ret = i2c.read(data, 2);
  return ret;
}
