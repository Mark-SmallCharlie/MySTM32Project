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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // 使能TIM3时钟（APB1总线）
    
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);   // 配置TIM3引脚重映射
    
    // 配置定时器时基（有错误，Period和Prescaler值不合理）
    TIM_TimeBaseStruct.TIM_Period = 999;                  // 自动重装载值（ARR）
    TIM_TimeBaseStruct.TIM_Prescaler = 0;                 // 预分频系数（PSC）
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频（不分频）
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数模式
    //TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
    
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStruct);          // 初始化调用
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);             // 使能更新中断
    TIM_Cmd(TIM3,ENABLE);                                // 使能TIM3
    
    // 配置输出比较通道2（PWM模式）
    TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;         // PWM模式1
    TIM_OCInitStruct.TIM_OCIdleState=TIM_OutputState_Enable;  // 空闲状态输出
    TIM_OCInitStruct.TIM_OutputState=TIM_OCInitStruct.TIM_Pulse=100;  // 有语法错误
    TIM_OC2Init(TIM3,&TIM_OCInitStruct);                 // 初始化通道2
    
    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);     // 使能预装载寄存器
    
    TIM_Cmd(TIM3,ENABLE);                                // 再次使能TIM3（重复）
}

// NVIC配置（注释掉的代码）
void NVICx_Init(void)
{
    //NVIC_InitTypeDef NVIC_InitStruct;
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    //NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    //NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    //NVIC_InitStruct.NVIC_IRQChannelSubPriority = 5;
    //NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStruct);
}
