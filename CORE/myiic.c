/*
* @Author: Amberimzyj
* @Date:   2018-03-25 17:26:56
* @Last Modified by:   Amberimzyj
* @Last Modified time: 2018-03-31 20:53:22
*/
#include "myiic.h"
#include "delay.h"


GPIO_InitTypeDef SDA_InitStructure;

void SDA_IN(void)
{
//    SDA_InitStructure.GPIO_Pin = GPIO_Pin_4;
//    SDA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    SDA_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
//    GPIO_Init(GPIOB, &SDA_InitStructure);   //初始化GPIOB
    PBout(4) = 1;
}

void SDA_OUT(void)
{
//    SDA_InitStructure.GPIO_Pin = GPIO_Pin_4;
//    SDA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    SDA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
//    GPIO_Init(GPIOB, &SDA_InitStructure);   //初始化GPIOB
}

//初始化IIC

void IIC_Init(void)
{

    //PB3,4s设置IO口
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //AFIO时钟使能
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);  //关闭JTAG功能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //PB时钟使能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);   //初始化GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);   //初始化GPIO
    GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4); //PB3,PB4输出高


}
//产生IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();  //SDA线输出
    IIC_SDA = 1;
    delay_us(5);
    IIC_SCL = 1;
    delay_us(5);
    IIC_SDA = 0; //START:when CLK is high,DATA change from high to low
    delay_us(5);
    IIC_SCL = 0; //钳住IIC总线，准备发送或接收数据
    delay_us(5);
}

//产生IIC停止信号
void IIC_Stop(void)
{
    SDA_OUT();  //SDA输出
    IIC_SCL = 0;
    delay_us(5);
    IIC_SDA = 0; //STOP:when CLK is high DATA change from low to high
    delay_us(5);
    IIC_SCL = 1;
    delay_us(5);
    IIC_SDA = 1; //发送IIC总线结束信号
    delay_us(5);
}

//等待应答信号到来
//返回值： 1，接收应答失败
//         0，接收应答成功
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    IIC_SDA = 1;
    delay_us(5);
    SDA_IN();  //SDA设置为输入
    IIC_SCL = 1;
    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 25)
        {
            //IIC_Stop();

            IIC_SCL = 0; // 时钟输出0
            return 1;
        }
    }
    IIC_SCL = 0; // 时钟输出0
    return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL = 0;
    delay_us(5);
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(5);
    IIC_SCL = 1;
    delay_us(5);
    IIC_SCL = 0;
    delay_us(5);
}

//不产生ACK应答
void IIC_NAck(void)
{
    IIC_SCL = 0;
    delay_us(5);
    SDA_OUT();
    IIC_SDA = 1;
    delay_us(5);
    IIC_SCL = 1;
    delay_us(5);
    IIC_SCL = 0;
    delay_us(5);
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL = 0; //拉低时钟开始数据传输
    delay_us(5);
    for (t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(5); //对TEA5767这三个延时都是必须的
        IIC_SCL = 1;
        delay_us(5);
        IIC_SCL = 0;
        delay_us(5);
    }
    IIC_SDA = 1;
    delay_us(5);
}

//读一个字节，ack=1时，发送ACK,ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();  //SDA设置为输入
    for (i = 0; i < 8; i++)
    {
        IIC_SCL = 0;
        delay_us(5);
        IIC_SCL = 1;
        delay_us(5);
        receive <<= 1;
        if (READ_SDA)
            receive++;
        delay_us(5);
    }
    if (!ack)
    {
        IIC_NAck();  //发送NACK
    }
    else
        IIC_Ack();  //发送ACK
    delay_us(5);
    return  receive;
}
