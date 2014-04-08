#include "../src/maa.h"

int
main ()
{
  maa::I2C i2c(28, 27);
  int addr = 0x62;
  char data[2];
  int ret = i2c.read(addr, data, 2);
  return ret;
}
