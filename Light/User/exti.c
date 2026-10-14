#include "exti.h"

void EXTIx_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 使能AFIO时钟（用于引脚重映射）
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);  // 配置GPIOE第4引脚为外部中断源
    
    // 配置外部中断线4
    EXTI_InitStruct.EXTI_Line = EXTI_Line4;               // 选择中断线4
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;      // 中断模式（非事件模式）
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  // 下降沿触发
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;                // 使能中断线
    EXTI_Init(&EXTI_InitStruct);                          // 使能AFIO时钟
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);  // 配置GPIOE引脚4作为外部中断源
    
		
}

// NVIC配置
//void NVICx_Init(void)
//{
//    NVIC_InitTypeDef NVIC_InitStruct;
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  // 设置中断优先级分组
//    NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;    // 选择EXTI4中断通道
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;  // 抢占优先级1
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 5;   // 子优先级5
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // 使能中断通道
//    NVIC_Init(&NVIC_InitStruct);
//}
