#include "stm32f10x.h"
#include "DHT11.h"

#include "delay.h"
#include "stdio.h"
#include "usart.h"

void DHT11_I(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin= DHT11_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
	
}
void DHT11_O(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(DHT11_PORT,&GPIO_InitStructure);

}
void DHT11_RST(void)
{
	DHT11_O();
	DHT11_L;
	delay_ms(20);
	DHT11_H;
	delay_us(30);
	
}
u8 DHT11_Check(void)
{
	u8 retry=0;
	DHT11_I();
	while(DHT11_Data ==1 && retry<100)
	{
		retry++;
		delay_us(1);
	}
	if(retry>=100)
		return 1;
	else retry=0;
	while(DHT11_Data==0&&retry<100)
	{
		retry++;
		delay_us(1);
		
	}
	if(retry>=100)
		return 1;
	else retry=0;
	return 0;
}
u8 DHT11_Read_Bit(void)
{
	while(DHT11_Data==1);
	while(DHT11_Data==0);
	delay_us(40);
	if(DHT11_Data==1)
		return 1;
	else return 0;
}
u8 DHT11_Read_Byte(void)
{
	u8 i=0;
	u8 dat=0;
	
	for(i=0;i<8;i++)
	{
		dat<<=1;
		dat|=DHT11_Read_Bit();
		
	}
	return dat;
}
u8 DHT11_Read_Data(u8*sd,u8*wd)
{
	u8 buf[5];
	u8 i=0;
	DHT11_RST();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_Read_Byte();
		}
		if(buf[0]+buf[1]+buf[2]+buf[3]==buf[4])
		{
		*sd=buf[0];
		*wd=buf[2];
		return 0;
		}
		else
		{
		return 1;
		}
	}
	else
	{
		return 1;
	}
}
void DHT11_Test(void)
{
	u8 sd;
	u8 wd;
	if(DHT11_Read_Data(&sd,&wd)==0)
	{
		printf("当前环境相对湿度：%d%%\r\n",sd);
		printf("当前环境温度：%d ℃\r\n",wd);
		printf("\r\n");
	}
	else
	{
		printf("DHT11 温湿度传感器检测失败！\r\n");
	}
	delay_ms(1000);
}



