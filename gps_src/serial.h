#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <inttypes.h>

#ifndef PORTNAME
#define PORTNAME "/dev/ttyS0"//for zero and 3
//for 2 ttyAMA0
#endif

void serial_init(void);
void serial_config(void);
void serialFlush (void);
void serial_println(const char *, int);
int serial_readln(char *, int);
void serial_close(void);

#endif
