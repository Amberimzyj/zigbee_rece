/*
* @Author: Amberimzyj
* @Date:   2018-04-08 15:22:01
* @Last Modified by:   Amberimzyj
* @Last Modified time: 2018-04-29 19:48:24
*/
#include "ESP8266.h"
#include "usart.h"
#include "delay.h"

//软件复位，模块断电初始化
//void ESP8266_Reset(void)
//{
//    _set_FAUL TMASK(1);
//    NVIC_SystemReset();
//}
void TCP_Con(void)
{
    // USART_PutStr(USART2, "\rAT+CWMODE=1\r\n");  //设置为sta模式
    delay_nms(3);
    USART_PutStr(USART2, "AT+CWLAP\r\n");     //搜索附近WIFI网络
    delay_nms(5);
    // USART_PutStr(USART2, "AT+CWJAP=\"Amber iPhone\",\"1234567888\"\r\n");  //连接WLAN
    // delay_nms(7);
    USART_PutStr(USART2, "AT+CIPMUX=0\r\n");  //设置单连接
    delay_nms(5);
    USART_PutStr(USART2, "AT+CIPSTART=\"TCP\",\"172.20.10.10\",8088\r\n");     //发起TCP连接请求
    delay_nms(5);
    USART_PutStr(USART2, "AT+CIPMODE=1\r\n");  //开启透传模式
    delay_nms(3);
    USART_PutStr(USART2, "AT+CIPSEND\r\n");  //开启透传模式
    delay_nms(3);
}

void TCP_Send(float data)
{
    USART_PutStr(USART2, "data");
    delay_nms(3);
}

void TCP_Close(void)
{
    USART_PutStr(USART2, "AT+CIPCLOSE");
    delay_nms(3);
}

void delay_nms(u8 n)
{
    for (u8 i = 0; i < n; i++)
        delay_ms(1000);
}
