#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

// 函数声明
void USART_Config(void);
void NVICx_Init(void);
void USART_SendChar(uint8_t ch);
void USART_SendString(uint8_t *str);
void USART_user(void);
uint8_t USART_IsDataReceived(void);
uint8_t* USART_GetReceivedData(void);

// 外部变量声明
extern uint8_t rx_buffer[64];
extern uint16_t rx_index;
extern volatile uint8_t rx_complete;

#endif
