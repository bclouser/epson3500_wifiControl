#ifndef _SERIAL_CONTROL_H
#define _SERIAL_CONTROL_H

#include "ets_sys.h"

void SerialControl_init(void);

void SerialControl_write(const char* buf);

// returns the number of bytes successfully read
int SerialControl_read(char* buf, int len);


#endif