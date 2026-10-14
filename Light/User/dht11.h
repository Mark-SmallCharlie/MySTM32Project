#ifndef _DHT11_H
#define _DHT11_H
#include "stm32f10x.h"
#define DHT11_PIN GPIO_Pin_1
#define DHT11_PORT GPIOA
#define DHT11_RCC_APB2Periph RCC_APB2Periph_GPIOA
#define DHT11_L GPIO_ResetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_H GPIO_SetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_Data GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN)

void DHT11_Test(void);


#endif 

