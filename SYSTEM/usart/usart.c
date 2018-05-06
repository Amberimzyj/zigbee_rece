#include "sys.h"
#include "usart.h"
#include "string.h"
//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                   //ucos 使用     
#endif
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART3_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
int _sys_exit(int x)
{
    x = x;
    return x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);//循环发送,直到发送完毕
    USART3->DR = (u8) ch;
    return ch;
}
#endif

/*使用microL                                                  ib的方法*/
/*
int fputc(int ch, FILE *f)
{
USART_SendData(USART3, (uint8_t) ch);

while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {}

   return ch;
}
int GetKey (void)  {

   while (!(USART3->SR & USART_FLAG_RXNE));

   return ((int)(USART3->DR & 0x1FF));
}
*/

#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u16 USART3_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，  接收完成标志
//bit14，  接收到0x0d
//bit13~0，    接收到的有效字节数目
u16 USART3_RX_STA = 0;     //接收状态标记

void usart3_init(u32 bound) {
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE); //使能USART3
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIOB时钟

    //USART3_TX   GPIOB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10

    //USART3_RX     GPIOB.11初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.10

    //USART3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART3, ENABLE);                    //使能串口3

}

//串口3中断
void USART3_IRQHandler(void)                    //串口3中断服务程序
{
    u8 Res;
#if SYSTEM_SUPPORT_OS       //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntEnter();
#endif
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART3); //读取接收到的数据


        if ((USART3_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (USART3_RX_STA & 0x4000) //接收到了0x0d
            {
                if (Res != 0x0a)USART3_RX_STA = 0; //接收错误,重新开始
                else USART3_RX_STA |= 0x8000; //接收完成了
            }
            else //还没收到0X0D
            {
                if (Res == 0x0d)USART3_RX_STA |= 0x4000;
                else
                {
                    USART3_RX_BUF[USART3_RX_STA & 0X3FFF] = Res ;
                    USART3_RX_STA++;
                    if (USART3_RX_STA > (USART_REC_LEN - 1))USART3_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }
    }
#if SYSTEM_SUPPORT_OS   //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntExit();
#endif
}
#endif


#if EN_USART2_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u16 USART2_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，  接收完成标志
//bit14，  接收到0x0d
//bit13~0，    接收到的有效字节数目
u16 USART2_RX_STA = 0;     //接收状态标记
//USART2端口设置
void usart2_init(u32 bound) {
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //使能USART2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟

    //USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2

    //USART2_RX     GPIOA.3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA.3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3

    //USART1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    //使能串口1

}

//串口2中断
void USART2_IRQHandler(void)                    //串口2中断服务程序
{
    u8 Res;
#if SYSTEM_SUPPORT_OS       //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntEnter();
#endif
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res = USART_ReceiveData(USART2); //读取接收到的数据

        if ((USART2_RX_STA & 0x8000) == 0) //接收未完成
        {
            if (USART2_RX_STA & 0x4000) //接收到了0x0d
            {
                if (Res != 0x0a)USART2_RX_STA = 0; //接收错误,重新开始
                else USART2_RX_STA |= 0x8000; //接收完成了
            }
            else //还没收到0X0D
            {
                if (Res == 0x0d)USART2_RX_STA |= 0x4000;
                else
                {
                    USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res ;
                    USART2_RX_STA++;
                    if (USART2_RX_STA > (USART_REC_LEN - 1))USART2_RX_STA = 0; //接收数据错误,重新开始接收
                }
            }
        }
    }
#if SYSTEM_SUPPORT_OS   //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntExit();
#endif
}
#endif

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u16 USART1_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，  接收完成标志
//bit14，  接收到0x0d
//bit13~0，    接收到的有效字节数目
u16 USART1_RX_STA = 0;     //接收状态标记
//USART1端口设置
void usart1_init(u32 bound) {
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //使能USART2、使能GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX     GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA.10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10

    //USART1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口1

}

//定义全局数组
// u8 usartrece[13] = {0};
// u8 len = 0;

//串口1中断
u8 USART_GetChar(USART_TypeDef* USARTx);
u8 flag = 0;
u8 data[13] = {0};
void USART1_IRQHandler(void)                    //串口2中断服务程序
{
    u8 Res;
    static u8 len = 0;
#if SYSTEM_SUPPORT_OS       //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntEnter();
#endif
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收中断
    {
        Res = USART_ReceiveData(USART1);
        if (flag == 0 && Res == 0xFE)
        {
            data[0] = Res;
            len = 1;
            flag = 1;
        }
        else if (flag == 1 && Res == 0xFF)
        {
            flag = 0;
            DealData();
            data[len] = Res;
            len = 0;
        }
        else if (flag == 1)
        {
            data[len] = Res;
            len++;
            // if (len == 12)
            // {
            //     flag = 0;
            //     len = 0;
            // }
        }
        // Res = USART_ReceiveData(USART1); //读取接收到的数据
        // // usartrece[len] = Res; //将串口接收到的数据放到缓冲数组中

        // // if (Res != 0xFF && len < 13)
        // // {
        // //     usartrece[len] = Res;
        // //     len++;
        // // }
        // // else if (Res == 0xff || len >= 13)
        // // {
        // //     len = 0;
        // //     memset(usartrece, 0, 13);
        // // }
        // if ((USART1_RX_STA & 0x8000) == 0) //接收未完成
        // {
        //     if (USART1_RX_STA & 0x4000) //接收到了0x0d
        //     {
        //         if (Res != 0x0a)USART1_RX_STA = 0; //接收错误,重新开始
        //         // len = 0;
        //         // usartrece[12] = {0};
        //         else USART1_RX_STA |= 0x8000; //接收完成了
        //     }
        //     else //还没收到0X0D
        //     {
        //         if (Res == 0x0d)USART1_RX_STA |= 0x4000;
        //         else
        //         {
        //             USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = Res ;
        //             USART1_RX_STA++;
        //             // usartrece[len]=Res;
        //             // len++;
        //             if (USART1_RX_STA > (USART_REC_LEN - 1))
        //             {
        //                 USART1_RX_STA = 0; //接收数据错误,重新开始接收
        //                 // len=0;
        //             }
        //         }
        //     }
        // }
    }
#if SYSTEM_SUPPORT_OS   //如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
    OSIntExit();
#endif
}
#endif


// u8 USART_GetChar(USART_TypeDef* USARTx)
// {
//     return USART_ReceiveData(USARTx);
// }

void USART_PutChar(USART_TypeDef* USARTx, uint8_t Data)
{
    USART_SendData(USARTx, Data);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET) {}
}

void USART_PutStr(USART_TypeDef* USARTx, uint8_t *str)
{
    while (0 != *str)
    {
        USART_PutChar(USARTx, *str);
        str++;
    }
}

void PutCharTemp(float data)
{
    char da[20];
    USART_PutStr(USART2, "当前温度为：");
    sprintf(da, "%g ℃\r\n", data);
    USART_PutStr(USART2, da);
}

void PutCharHumid(float data)
{
    char da[20];
    USART_PutStr(USART2, "当前湿度为：");
    sprintf(da, "%g %%RH\r\n", data);
    USART_PutStr(USART2, da);
}

void PutCharLight(float data)
{
    char da[20];
    USART_PutStr(USART2, "当前光强为：");
    sprintf(da, "%g lx\r\n\r\n", data);
    USART_PutStr(USART2, da);
}
