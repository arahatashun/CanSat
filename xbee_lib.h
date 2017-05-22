//xbee ライブラリヘッダファイル xbee_lib.h

#ifndef XBEELIB_H
#define XBEELIB_H

typedef unsigned char byte; 			// Arduinoでも必要

byte xbee_init(const byte port);
void xbee_from(byte *address);
byte xbee_atnj(const byte timeout);
byte xbee_ratnj(const byte *address, const byte timeout);
byte xbee_uart(const byte *address, const char *in);

#endif
