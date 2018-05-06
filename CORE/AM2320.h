#ifndef __AM2320__H
#define __AM2320__H
#include "myiic.h"

#define AM2320_Init() IIC_Init()

void AM2320_Wakeup(void);  //唤醒传感器
void AM2320_Send_Read(void);  //发送读指令
u32 AM2320_Read(void);  //从AM2320读取数据
#endif
