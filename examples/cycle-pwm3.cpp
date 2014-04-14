#include <unistd.h>

#include "maa.h"

int
main ()
{
  maa::PWM pwm(0, 3);
  pwm.period_us(200);
  pwm.enable(1);

  float value = 0.0f;

  while(1) {
    value = value + 0.01f;
    pwm.write(value);
    usleep(50000);
    if (value >= 1.0f) {
        value = 0.0f;
    }
    float output = pwm.read();
  }
  return 0;
}
