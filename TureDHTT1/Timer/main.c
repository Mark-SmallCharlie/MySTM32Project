#include "delay.h"
#include "led.h"
#include "key.h"
#include "DHT11.h"
#include "stdio.h"
#include "exti.h"
#include "timer.h"
#include "USART.h"


int fput(int c,FILE* stream)
{
	USART1->DR=c;
	while(!(USART1->SR&(1<<7))){};
  return c;
}

int main(void)
{
USART_user();
RCC_APB2PeriphClockCmd(DHT11_RCC_APB2Periph,ENABLE);
while(1)
	{
		DHT11_Test();
		delay_ms(1000);
	}	
}
		

	

