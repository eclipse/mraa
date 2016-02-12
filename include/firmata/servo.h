
#ifndef		__H_FIRMATA_SERVO__
#define		__H_FIRMATA_SERVO__

#include	"firmata.h"

typedef struct	s_servo
{
  t_firmata	*firmata;
  int		pin;
}		t_servo;

t_servo		*servo_attach(t_firmata *firmata, int pin);
int		servo_write(t_servo *servo, int value);

#endif
