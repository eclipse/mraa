
#ifndef __H_FIRMATAC_SERIAL_
#define __H_FIRMATAC_SERIAL_

#include <stdint.h>

#include <termios.h>

typedef struct		s_serial
{
  int			port_is_open;
  char			*port_name;
  int			baud_rate;
  char			*error_msg;

  int			port_fd;
  struct termios	settings_orig;
  struct termios	settings;
  int			tx;
  int			rx;
}			t_serial;

t_serial		*serial_new();
int             serial_open(t_serial *serial, char *name);
int		serial_setBaud(t_serial *serial, int baud);
int		serial_read(t_serial *serial, void *ptr, int count);
int		serial_write(t_serial *serial, void *ptr, int len);
int		serial_waitInput(t_serial *serial, int msec);
int		serial_discardInput(t_serial *serial);
void		serial_flushOutput(t_serial *serial);
int		serial_setControl(t_serial *serial, int dtr, int rts);

#endif
