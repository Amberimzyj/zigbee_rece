/*
* @Author: Amberimzyj
* @Date:   2018-03-25 17:26:56
* @Last Modified by:   Amberimzyj
* @Last Modified time: 2018-03-31 17:54:24
*/
#include "myiic1.h"
#include "delay.h"


GPIO_InitTypeDef SDA_IN1itStructure;

void SDA_IN1(void)
{
//    SDA_IN1itStructure.GPIO_Pin = GPIO_Pin_4;
//    SDA_IN1itStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    SDA_IN1itStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
//    GPIO_Init(GPIOB, &SDA_IN1itStructure);   //初始化GPIOB
    PAout(6) = 1;
}

void SDA_OUT1(void)
{
//    SDA_IN1itStructure.GPIO_Pin = GPIO_Pin_4;
//    SDA_IN1itStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    SDA_IN1itStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
//    GPIO_Init(GPIOB, &SDA_IN1itStructure);   //初始化GPIOB
}

//初始化IIC

void IIC_Init1(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //PA时钟使能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIO
    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6); //PA5,PA6输出高
    GPIO_ResetBits(GPIOA, GPIO_Pin_7); //PA7输出低，ADDR接地
}
//产生IIC起始信号
void IIC_Start1(void)
{
    SDA_OUT1();  //SDA线输出
    IIC_SDA1 = 1;
    delay_us(5);
    IIC_SCL1 = 1;
    delay_us(5);
    IIC_SDA1 = 0; //START:when CLK is high,DATA change from high to low
    delay_us(5);
    IIC_SCL1 = 0; //钳住IIC总线，准备发送或接收数据
    delay_us(5);
}

//产生IIC停止信号
void IIC_Stop1(void)
{
    SDA_OUT1();  //SDA输出
    IIC_SCL1 = 0;
    delay_us(5);
    IIC_SDA1 = 0; //STOP:when CLK is high DATA change from low to high
    delay_us(5);
    IIC_SCL1 = 1;
    delay_us(5);
    IIC_SDA1 = 1; //发送IIC总线结束信号
    delay_us(5);
}

//等待应答信号到来
//返回值： 1，接收应答失败
//         0，接收应答成功
u8 IIC_Wait_Ack1(void)
{
    u8 ucErrTime = 0;
    IIC_SDA1 = 1;
//    delay_us(5);
    SDA_IN1();  //SDA设置为输入
    IIC_SCL1 = 1;
    while (READ_SDA1)
    {
        ucErrTime++;
        if (ucErrTime > 25)
        {
            //IIC_Stop();

            IIC_SCL1 = 0; // 时钟输出0
            return 1;
        }
    }
    IIC_SCL1 = 0; // 时钟输出0
    delay_us(5);
    return 0;
}
//产生ACK应答
void IIC_Ack1(void)
{
    IIC_SCL1 = 0;
    delay_us(5);
    SDA_OUT1();
    IIC_SDA1 = 0;
    delay_us(5);
    IIC_SCL1 = 1;
    delay_us(5);
    IIC_SCL1 = 0;
    delay_us(5);
}

//不产生ACK应答
void IIC_NAck1(void)
{
    IIC_SCL1 = 0;
    delay_us(5);
    SDA_OUT1();
    IIC_SDA1 = 1;
    delay_us(5);
    IIC_SCL1 = 1;
    delay_us(5);
    IIC_SCL1 = 0;
    delay_us(5);
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte1(u8 txd)
{
    u8 t;
    SDA_OUT1();
    IIC_SCL1 = 0; //拉低时钟开始数据传输
    delay_us(5);
    for (t = 0; t < 8; t++)
    {
        IIC_SDA1 = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(5); //对TEA5767这三个延时都是必须的
        IIC_SCL1 = 1;
        delay_us(5);
        IIC_SCL1 = 0;
        delay_us(5);
    }
    IIC_SDA1 = 1;
    delay_us(5);
}

//读一个字节，ack=1时，发送ACK,ack=0，发送nACK
u8 IIC_Read_Byte1(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN1();  //SDA设置为输入
    for (i = 0; i < 8; i++)
    {
        IIC_SCL1 = 0;
        delay_us(5);
        IIC_SCL1 = 1;
        delay_us(5);
        receive <<= 1;
        if (READ_SDA1)
            receive++;
        delay_us(5);
    }
    if (!ack)
    {
        IIC_NAck1();  //发送NACK
    }
    else
        IIC_Ack1();  //发送ACK
    delay_us(5);
    return  receive;
}
