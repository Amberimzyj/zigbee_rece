#ifndef _ESP8266_H
#define _ESP8266_H
#include "stdio.h"
#include "sys.h"

void ESP8266_Reset(void);
void TCP_Con(void);
void TCP_Send(float data);
void TCP_Close(void);
void delay_nms(u8 n);

#endif
