
#include	"firmata/servo.h"

#include	<stdlib.h>
#include	<stdio.h>

t_servo		*servo_attach(t_firmata *firmata, int pin)
{
  t_servo	*res;

  if (!firmata || !firmata->isReady)
    {
      perror("servo_new::Firmata is not ready");
      return (NULL);
    }
  res = malloc(sizeof(t_servo));
  if (!res)
    {
      perror("servo_new::malloc failed");
      return (NULL);
    }
  res->firmata = firmata;
  res->pin = pin;
  firmata_pinMode(res->firmata, pin, MODE_SERVO);
  return (res);
}

int		servo_write(t_servo *servo, int value)
{
  return (firmata_analogWrite(servo->firmata, servo->pin, value));
}
