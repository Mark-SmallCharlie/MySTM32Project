#ifndef __UART_H
#define __UART_H
#include "stm32f10x.h"


void USART_Config(void);
void USART_user(void);
void USART_Configuration(void);
void NVICx_Init(void);
void USART1_IRQHandler(void);

#endif

