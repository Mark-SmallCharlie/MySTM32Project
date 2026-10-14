#include "timer.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "misc.h"
void TIMER_Init(void)
{
	//TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //使用复用时钟
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);   //设定定时器的映射方式
	
	TIM_TimeBaseStruct.TIM_Period = 999;
	TIM_TimeBaseStruct.TIM_Prescaler = 0;
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	//TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStruct);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_OutputState=TIM_OCInitStruct.TIM_Pulse=100;
	TIM_OC2Init(TIM3,&TIM_OCInitStruct);
	
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);  //使用预装载寄存器的值
	
	TIM_Cmd(TIM3,ENABLE);  //使用定时器
}

//void NVICx_Init(void)
//{
//	//NVIC_InitTypeDef NVIC_InitStruct;
//	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//	//NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
//	//NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
//	//NVIC_InitStruct.NVIC_IRQChannelSubPriority = 5;
//	//NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	//NVIC_Init(&NVIC_InitStruct);
//}

