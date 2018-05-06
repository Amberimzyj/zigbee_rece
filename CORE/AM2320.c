/*
* @Author: Amberimzyj
* @Date:   2018-03-26 19:46:39
* @Last Modified by:   Amberimzyj
* @Last Modified time: 2018-03-29 14:22:40
*/
#include "AM2320.h"
#include "delay.h"
#include "myiic.h"
#include "usart.h"

//唤醒AM2320
void AM2320_Wakeup(void)
{
    IIC_Start();
    IIC_Send_Byte(0xB8);   //发送传感器地址
    IIC_Wait_Ack();
    delay_ms(2);
    IIC_Stop();   //停止信号
}

//向AM2320发送读指令
void AM2320_Send_Read(void)
{
    IIC_Start();
    IIC_Send_Byte(0xB8); //发送传感器地址+写指令
    IIC_Wait_Ack();
//        printf("\r\nAlready sent 0xB8!\r\n");
    IIC_Send_Byte(0x03);  //发送功能码
    IIC_Wait_Ack();

//            printf("\r\nAlready sent 0x03!\r\n");
    IIC_Send_Byte(0x00);  //发送起始地址
    IIC_Wait_Ack();

//                printf("\r\nAlready sent 0x00!\r\n");
    IIC_Send_Byte(0x04);  //发送寄存器长度
    IIC_Wait_Ack();
    //delay_ms(3);
    IIC_Stop();
}

//从AM2320读取数据
u32 AM2320_Read(void)
{
    u8 i;
    u32 receivedata = 0;
    IIC_Start();
    IIC_Send_Byte(0xB9); //发送传感器地址+读指令
    IIC_Wait_Ack();
    delay_us(35);
//    IIC_Send_Byte(0x03);  //发送功能码
//    IIC_Send_Byte(0x00);  //发送起始地址
//    IIC_Send_Byte(0x04);  //发送寄存器长度

//        printf("\r\nAlready received ACK!\r\n");

    IIC_Read_Byte(1);//读取一个字节，发送ACK
    IIC_Read_Byte(1);//读取一个字节，发送ACK
    for (i = 0; i < 4; i++)
    {
        receivedata <<= 8;
        receivedata |= IIC_Read_Byte(1);//读取一个字节

    }
    IIC_Read_Byte(1);//读取一个字节，发送ACK
    IIC_Read_Byte(0);//读取一个字节，发送NAK

    //IIC_NAck();
    IIC_Stop();


    return receivedata;
}

