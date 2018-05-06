/*
* @Author: Amberimzyj
* @Date:   2018-03-26 19:46:39
* @Last Modified by:   Amberimzyj
* @Last Modified time: 2018-03-31 18:33:16
*/
#include "GY-30.h"
#include "delay.h"
#include "myiic1.h"
#include "usart.h"

//向GY-30发送模式指令
void GY_30_SendMode(u8 mode)
{

    IIC_Start1();
    IIC_Send_Byte1(0x46);   //发送传感器地址+写指令
    delay_us(5);
    IIC_Wait_Ack1();
    IIC_Send_Byte1(mode);   //发送模式指令
    delay_us(5);
    IIC_Wait_Ack1();
    IIC_Stop1();   //停止信号
}


// 初始化GY-30
void GY_30_Init(void)
{
    GY_30_SendMode(0x01);  //上电初始化
}

//从GY-30读取数据
u16 GY_30_Read(void)
{
    u16 receivedata = 0;
    IIC_Start1();
    IIC_Send_Byte1(0x47); //发送传感器地址+读指令
    IIC_Wait_Ack1();
    //delay_us(35);

    receivedata |= IIC_Read_Byte1(1);//读取一个字节,并发送ACK
    receivedata <<= 8;
    receivedata |= IIC_Read_Byte1(0); //读取一个字节，并发送NAK

    IIC_Stop1();

    return receivedata;
}

