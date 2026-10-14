//#ifndef _DEALF_H
#define _DEAlY_H
#include "stm32f10x.h"

#include "delay.h"
void delay(u32 nCount)
{
	for(; nCount != 0; nCount-- );
}
void delay_us(u32 nus)
{
	u32 temp;
	SysTick->VAL=0x00;
	SysTick->LOAD=9*nus;
	SysTick->CTRL=0x01;
	
	do
		temp=SysTick->CTRL;
	while((temp&0x01)&&(!temp&(1<<16)));
	SysTick->VAL=0x00;
	SysTick->CTRL=0x00;
	
}
void delay_ms(u16 nms)
{
	u16 temp;
	SysTick->VAL=0x00;
	SysTick->LOAD=900;
	SysTick->CTRL=0x01;

	do
		temp=SysTick->CTRL;
	while((temp&0x01)&&(!temp&(1<<16)));
	SysTick->VAL=0x00;
	SysTick->CTRL=0x00;
}

