#include "led.h"
//初始化PB12为输出口，并使能这两个口的时钟
//LED IO初始化

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOF时钟
	//GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //LED2对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //100MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIO
	GPIO_SetBits(GPIOB, GPIO_Pin_12 ); //GPIOB12设置高，灯灭
}
