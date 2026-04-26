#ifndef __BH1750_H
#define __BH1750_H
#include "stm32f10x.h"

void BH1750_Init(void);
float BH1750_ReadLux_Safe(void);           // 修改为安全版本
void Auto_Dimming_Logic_Safe(float lux);   // 修改为安全版本

#endif
