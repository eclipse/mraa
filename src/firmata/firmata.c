
#include	"firmata/serial.h"
#include	"firmata/firmata.h"

#include	<string.h>
#include	<stdlib.h>
#include	<stdio.h>

t_firmata	*firmata_new(char *name)
{
  t_firmata	*res;

  printf("Opening device at: %s\n", name);
  res = malloc(sizeof(t_firmata));
  if (!res)
    {
      perror("firmata_new::Failed malloc");
      return (NULL);
    }
  res->serial = serial_new();
  if (!res->serial)
    {
      perror("firmata_new::Failed malloc");
      return (NULL);
    }
  serial_open(res->serial, name);
  firmata_initPins(res);
  serial_setBaud(res->serial, 57600);
  firmata_askFirmware(res);
  printf("Device opened at: %s\n", name);
  return (res);
}

int		firmata_pull(t_firmata *firmata)
{
  uint8_t	buff[FIRMATA_MSG_LEN];
  int		r;

  r = serial_waitInput(firmata->serial, 40);
  if (r > 0) {
    r = serial_read(firmata->serial, buff, sizeof(buff));
    if (r < 0) {
      return (0);
    }
    if (r > 0) {
      firmata_parse(firmata, buff, r);
      return (r);
    }
  } else if (r < 0) {
    return (r);
  }
}

void		firmata_parse(t_firmata *firmata, const uint8_t *buf, int len)
{
  const uint8_t	*p;
  const uint8_t	*end;

  p = buf;
  end = p + len;
  for (p = buf; p < end; p++) {
    uint8_t msn = *p & 0xF0;
    if (msn == 0xE0 || msn == 0x90 || *p == 0xF9) {
      firmata->parse_command_len = 3;
      firmata->parse_count = 0;
    } else if (msn == 0xC0 || msn == 0xD0) {
      firmata->parse_command_len = 2;
      firmata->parse_count = 0;
    } else if (*p == FIRMATA_START_SYSEX) {
      firmata->parse_count = 0;
      firmata->parse_command_len = sizeof(firmata->parse_buff);
    } else if (*p == FIRMATA_END_SYSEX) {
      firmata->parse_command_len = firmata->parse_count + 1;
    } else if (*p & 0x80) {
      firmata->parse_command_len = 1;
      firmata->parse_count = 0;
    }
    if (firmata->parse_count < (int) sizeof(firmata->parse_buff)) {
      firmata->parse_buff[firmata->parse_count] = (uint8_t) (*p);
      firmata->parse_count++;
    }
    if (firmata->parse_count == firmata->parse_command_len) {
      firmata_endParse(firmata);
      firmata->parse_count = 0;
      firmata->parse_command_len = 0;
    }
  }
}

void		firmata_endParse(t_firmata *firmata)
{
  uint8_t	cmd = (firmata->parse_buff[0] & 0xF0);
  int		pin;

  if (cmd == 0xE0 && firmata->parse_count == 3) {
    int analog_ch = (firmata->parse_buff[0] & 0x0F);
    int analog_val = firmata->parse_buff[1] | (firmata->parse_buff[2] << 7);
    for (pin = 0; pin < 128; pin++) {
      if (firmata->pins[pin].analog_channel == analog_ch) {
	firmata->pins[pin].value = analog_val;
	return;
      }
    }
    return;
  }
  if (cmd == 0x90 && firmata->parse_count == 3) {
    int port_num = (firmata->parse_buff[0] & 0x0F);
    int port_val = firmata->parse_buff[1] | (firmata->parse_buff[2] << 7);
    int pin = port_num * 8;
    int mask;
    for (mask=1; mask & 0xFF; mask <<= 1, pin++) {
      if (firmata->pins[pin].mode == MODE_INPUT) {
	uint32_t val = (port_val & mask) ? 1 : 0;
	firmata->pins[pin].value = val;
      }
    }
    return;
  }
  if (firmata->parse_buff[0] == FIRMATA_START_SYSEX && firmata->parse_buff[firmata->parse_count - 1] == FIRMATA_END_SYSEX) {
    if (firmata->parse_buff[1] == FIRMATA_REPORT_FIRMWARE) {
      int len=0;
      int i;
      for (i = 4; i < firmata->parse_count-2; i += 2) {
	firmata->firmware[len++] = (firmata->parse_buff[i] & 0x7F)
	  | ((firmata->parse_buff[i+1] & 0x7F) << 7);
      }
      firmata->firmware[len++] = '-';
      firmata->firmware[len++] = firmata->parse_buff[2] + '0';
      firmata->firmware[len++] = '.';
      firmata->firmware[len++] = firmata->parse_buff[3] + '0';
      firmata->firmware[len++] = 0;
      printf("Name :: %s\n", firmata->firmware);
      // query the board's capabilities only after hearing the                              
      // REPORT_FIRMWARE message.  For boards that reset when                               
      // the port open (eg, Arduino with reset=DTR), they are                               
      // not ready to communicate for some time, so the only                                
      // way to reliably query their capabilities is to wait                                
      // until the REPORT_FIRMWARE message is heard.                                        
      uint8_t buf[80];
      len=0;
      buf[len++] = FIRMATA_START_SYSEX;
      buf[len++] = FIRMATA_ANALOG_MAPPING_QUERY; // read analog to pin # info                       
      buf[len++] = FIRMATA_END_SYSEX;
      buf[len++] = FIRMATA_START_SYSEX;
      buf[len++] = FIRMATA_CAPABILITY_QUERY; // read capabilities                                   
      buf[len++] = FIRMATA_END_SYSEX;
      for (i = 0; i < 16; i++) {
	buf[len++] = 0xC0 | i;  // report analog                                      
	buf[len++] = 1;
	buf[len++] = 0xD0 | i;  // report digital                                     
	buf[len++] = 1;
      }
      firmata->isReady = 1;
      serial_write(firmata->serial, buf, len);
    } else if (firmata->parse_buff[1] == FIRMATA_CAPABILITY_RESPONSE) {
      int pin, i, n;
      for (pin=0; pin < 128; pin++) {
	firmata->pins[pin].supported_modes = 0;
      }
      for (i=2, n=0, pin=0; i<firmata->parse_count; i++) {
	if (firmata->parse_buff[i] == 127) {
	  pin++;
	  n = 0;
	  continue;
	}
	if (n == 0) {
	  // first byte is supported mode                                       
	  firmata->pins[pin].supported_modes |= (1 << firmata->parse_buff[i]);
	}
	n = n ^ 1;
      }
      // send a state query for for every pin with any modes                                
      for (pin=0; pin < 128; pin++) {
	uint8_t buf[512];
	int len=0;
	if (firmata->pins[pin].supported_modes) {
	  buf[len++] = FIRMATA_START_SYSEX;
	  buf[len++] = FIRMATA_PIN_STATE_QUERY;
	  buf[len++] = pin;
	  buf[len++] = FIRMATA_END_SYSEX;
	}
	serial_write(firmata->serial, buf, len);
      }
    } else if (firmata->parse_buff[1] == FIRMATA_ANALOG_MAPPING_RESPONSE) {
      int pin = 0;
      int i;
      for (i = 2; i< firmata->parse_count - 1; i++) {
	firmata->pins[pin].analog_channel = firmata->parse_buff[i];
	pin++;
      }
      return;
    } else if (firmata->parse_buff[1] == FIRMATA_PIN_STATE_RESPONSE && firmata->parse_count >= 6) {
      int pin = firmata->parse_buff[2];
      firmata->pins[pin].mode = firmata->parse_buff[3];
      firmata->pins[pin].value = firmata->parse_buff[4];
      if (firmata->parse_count > 6) firmata->pins[pin].value |= (firmata->parse_buff[5] << 7);
      if (firmata->parse_count > 7) firmata->pins[pin].value |= (firmata->parse_buff[6] << 14);
    }
    return;
  }


}

void		firmata_initPins(t_firmata *firmata)
{
  int		i;

  firmata->parse_count = 0;
  firmata->parse_command_len = 0;
  firmata->isReady = 0;
  for (i = 0; i < 128; i++) {
    firmata->pins[i].mode = 255;
    firmata->pins[i].analog_channel = 127;
    firmata->pins[i].supported_modes = 0;
    firmata->pins[i].value = 0;
  }
}

int		firmata_askFirmware(t_firmata *firmata)
{
  uint8_t	buf[3];
  int		res;

  buf[0] = FIRMATA_START_SYSEX;
  buf[1] = FIRMATA_REPORT_FIRMWARE; // read firmata name & version                     
  buf[2] = FIRMATA_END_SYSEX;
  res = serial_write(firmata->serial, buf, 3);
  return (res);
}

int		firmata_pinMode(t_firmata *firmata, int pin, int mode)
{
  int		res;
  uint8_t	buff[4];

  firmata->pins[pin].mode = mode;
  buff[0] = FIRMATA_SET_PIN_MODE;
  buff[1] = pin;
  buff[2] = mode;
  printf("Setting pinMode at: %i with value: %i\n", pin, mode);
  res = serial_write(firmata->serial, buff, 3);
  return (res);
}

int		firmata_analogWrite(t_firmata *firmata, int pin, int value)
{
  int		res;

  uint8_t buff[3];
  printf("Writting analogWrite at: %i with value: %i\n", pin, value);
  buff[0] = 0xE0 | pin;
  buff[1] = value & 0x7F;
  buff[2] = (value >> 7) & 0x7F;
  res = serial_write(firmata->serial, buff, 3);
  return (res);
}

int		firmata_digitalWrite(t_firmata *firmata, int pin, int value)
{
  int		i;
  int		res;
  uint8_t	buff[4];

  if (pin < 0 || pin > 127)
    return (0);
  firmata->pins[pin].value = value;
  int port_num = pin / 8;
  int port_val = 0;
  for (i = 0; i < 8; i++) {
    int p = port_num * 8 + i;
    if (firmata->pins[p].mode == MODE_OUTPUT || firmata->pins[p].mode == MODE_INPUT)
      {
	if (firmata->pins[p].value)
	  {
	    port_val |= (1<<i);
	  }
      }
  }
  printf("Writting digitalWrite at: %i with value: %i\n", pin, value);
  buff[0] = FIRMATA_DIGITAL_MESSAGE | port_num;
  buff[1] = port_val & 0x7F;
  buff[2] = (port_val >> 7) & 0x7F;
  res = serial_write(firmata->serial, buff, 3);
  return (res);
}
