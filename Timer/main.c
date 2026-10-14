#include "delay.h"
#include "led.h"
#include "key.h"
#include "DHT11.h"
#include "stdio.h"
#include "exti.h"
#include "timer.h"
#include "USART.h"
#include "stm32f10x_gpio.h"


int fput(int c,FILE* stream)
{
	USART1->DR=c;
	while(!(USART1->SR&(1<<7))){};
  return c;
}

int main(void)
{
	RCC_APB2PeriphResetCmd();
	
	GPIO_InitTypeDef GPIO_Init1;
	USART_InitTypeDef ddd;
	ddd.USART_BaudRate=115200;
	ddd.USART_Mode=
	ddd.USART_WordLength=USART_WordLength_8b;
	GPIO_Init1.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init1.GPIO_Pin=GPIO_Pin_5;
	GPIO_Init1.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Init1);
	
	
}
		

	

