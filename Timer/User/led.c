#include "led.h"
void LED_Init(void)
{
	
	 // 初始化GPIOB的第5引脚 (LED1)
		GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;                 // 选择引脚5
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;          // 推挽输出模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;         // 50MHz速度
    GPIO_Init(GPIOB,&GPIO_InitStruct);                     // 初始化GPIOB
	
	GPIO_InitTypeDef PP;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
}
