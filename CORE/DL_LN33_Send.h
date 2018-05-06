
#include "stdio.h"
#include "sys.h"

u16 GetInfo(u8 port);
void SendInfo(u16 temp, u16 humid, u16 light);
u32 Char2Hex(u32 ch);

//typedef struct allda
//{
//    u8 head;
//    u8 length;
//    u8 src_port;
//    u8 dis_port;
//    u8 addr_L;
//    u8 addr_H;
//    u32 data;

//} allda;

#ifndef _DL_LN33_Send_H
#define _DL_LN33_Send_H
#endif
