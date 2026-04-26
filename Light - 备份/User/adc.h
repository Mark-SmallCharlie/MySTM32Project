#ifndef __ADC_H
#define __ADC_H	

#include "stm32f10x.h"

// 函数声明
void Adc_Init(void);                        // ADC1初始化
u16 Get_Adc(u8 ch);                         // 获得某个通道的单次ADC值
u16 Get_Adc_Average(u8 ch, u8 times);       // 获得某个通道多次采样的平均值，用于软件滤波防抖

#endif