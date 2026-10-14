#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f10x.h"
void TIM2_PWM_Init(u16 arr, u16 psc);
void LED_Set_PWM_Duty(u8 duty_percent);

// 新增的安全函数声明
void LED_Set_PWM_Duty_Safe(u8 target_duty);

#endif
