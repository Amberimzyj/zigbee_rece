/*
* @Author: Amberimzyj
* @Date:   2018-04-16 19:45:08
* @Last Modified by:   Amberimzyj
* @Last Modified time: 2018-04-17 21:45:26
*/
#include "DL_LN33_Send.h"
#include "usart.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"


//得到目的模块的基本信息
//u16 GetInfo(u8 port)
//{
//    u16 data = 0;
//    switch (port)
//    {
//    case 1:
//        USART_PutStr(USART1, "FE 05 90 91 00 00 01 FF\r\n");  // 获取本模块地址
//        delay_ms(10);
//        data = usartrece[7];
//        data <<= 8;
//        data = data | usartrece[8];
//        return data;
//    case 2:
//        USART_PutStr(USART1, "FE 05 90 91 00 00 02 FF\r\n");  // 获取本模块网络ID
//        delay_ms(10);
//        data = usartrece[7];
//        data <<= 8;
//        data = data | usartrece[8];
//        return data;
//    case 3:
//        USART_PutStr(USART1, "FE 05 90 91 00 00 03 FF\r\n");  // 获取本模块信道
//        delay_ms(10);
//        data = usartrece[7];
//        return data;
//    case 4:
//        USART_PutStr(USART1, "FE 05 90 91 00 00 04 FF\r\n");  // 获取本模块波特率
//        delay_ms(10);
//        data = usartrece[7];
//        return data;
//    }
//}



//将字符转换为16进制数
//u32 Char2Hex(u32 ch)
//{
//    if (ch >= '0' && ch <= '9')
//    {
//        return ch -= 0x30;
//    }
//    else if (ch >= 'A' && ch <= 'F')
//    {
//        return ch = ch - 'A' + 10;
//    }
//    else if (ch >= 'a' && ch <= 'f')
//    {
//        return ch = ch - 'a' + 10;
//    }
//    else
//    {
//        return -1;
//    }
//}


u8 allda[13] = {0XFE, 0X0A, 0X90, 0X91, 0X2F, 0X00, 0, 0, 0, 0, 0, 0, 0XFF};


//向网关发送信息
void SendInfo(u16 temp, u16 humid, u16 light)
{
    u16 t = 0;
    for (u8 i = 0; i < 13; i++)
    {
        if (i == 6)
        {
            t = temp & 0xff00;
            t >>= 8;
            allda[i] = t;
            USART_PutChar(USART1, allda[i]);
        }
        else if (i == 7)
        {
            t = temp & 0x00ff;
            allda[i] = t;
            USART_PutChar(USART1, allda[i]);
        }
        else if (i == 8)
        {
            t = humid & 0xff00;
            t >>= 8;
            allda[i] = t;
            USART_PutChar(USART1, allda[i]);
        }
        else if (i == 9)
        {
            t = humid & 0x00ff;
            allda[i] = t;
            USART_PutChar(USART1, allda[i]);
        }
        else if (i == 10)
        {
            t = light & 0xff00;
            t >>= 8;
            allda[i] = t;
            USART_PutChar(USART1, allda[i]);
        }
        else if (i == 11)
        {
            t = light & 0x00ff;
            allda[i] = t;
            USART_PutChar(USART1, allda[i]);
        }
        else
        {
            USART_PutChar(USART1, allda[i]);
        }
    }
}
