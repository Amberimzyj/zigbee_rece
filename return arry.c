/*
* @Author: Amberimzyj
* @Date:   2018-04-18 12:13:51
* @Last Modified by:   Amberimzyj
* @Last Modified time: 2018-04-18 16:56:34
*/
#include "DL_LN33_Rece.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"



char str_2[] = "temp";
char str_3[] = "humid";
char str_4[] = "light";


u8 (*ReadData())[6]
{

    u8 (*data)[6];
    data = (u8(*)[6])calloc(6, sizeof(char));
    if (USART_GetChar(USART1) == 0xFF)
    {

        if (USART_GetChar(USART1) == 0xFE)
        {
            for (u8 i = 0; i < 5; i++)
            {
                USART_GetChar(USART1);
            }
            for (u8 t = 0; t < 6; t++)
            {
                (*datas)[t] = USART_GetChar(USART1);
            }
            return data;
        }
        else
        {
            return 0;
        }
    }
}

float DealData(char str_1[])
{
    u8 (*data)[6];
    data = ReadData();
    u16 data1 = 0;
    float data2;
    if (strcmp(str_1, str_2) == 0)
    {
        data1 = data[0];
        data1 <<= 8;
        data1 |= data[1];
        return data2 = data1 / 10.0;
    }
    else if (strcmp(str_1, str_3) == 0)
    {
        data1 = data[2];
        data1 <<= 8;
        data1 |= data[3];
        return data2 = data1 / 10.0;
    }
    else if (strcmp(str_1, str_4) == 0)
    {
        data1 = data[4];
        data1 <<= 8;
        data1 |= data[5];
        return data2 = data1 / 1.2;
    }
    free(data);
}
