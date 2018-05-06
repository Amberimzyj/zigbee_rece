#ifndef __GY_30__H
#define __GY_30__H
#include "myiic.h"


void GY_30_SendMode(u8 mode);//向GY-30发送模式指令
void GY_30_Init(void);// 初始化GY-30
u16 GY_30_Read(void);  //从GY-30读取数据

#endif
