#ifndef _LED_H
#define _LED_H	 

#include "stm32f10x.h"

// 宏定义：方便后续在 main.c 中直接控制 LED 亮灭
#define LED_ON()  GPIO_ResetBits(GPIOC, GPIO_Pin_13) // PC13 输出低电平，LED 亮 (通常核心板为低电平驱动)
#define LED_OFF() GPIO_SetBits(GPIOC, GPIO_Pin_13)   // PC13 输出高电平，LED 灭

// 函数声明
void LED_Init(void);      // 初始化
void LED_Toggle(void);    // 状态翻转 (用于心跳灯)

#endif
