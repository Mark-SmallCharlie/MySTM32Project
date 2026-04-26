#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f10x.h"

#include "delay.h"

// 简单的软件延时函数（不精确）
void delay(u32 nCount)
{
    for(; nCount != 0; nCount-- );  // 通过循环计数实现延时
}

// 微秒级延时函数（使用SysTick定时器）
void delay_us(u32 nus)
{
    u32 temp;
    SysTick->VAL=0x00;              // 清空当前计数值
    SysTick->LOAD=9*nus;            // 设置重装载值（9MHz时钟：9*nus得到n微秒）
    SysTick->CTRL=0x01;             // 使能SysTick（不使用中断）
    
    do
        temp=SysTick->CTRL;         // 读取控制寄存器状态
    while((temp&0x01)&&(!temp&(1<<16)));  // 等待计数完成（检查使能位和COUNTFLAG）
    
    SysTick->VAL=0x00;              // 清空计数值
    SysTick->CTRL=0x00;             // 关闭SysTick
}

// 毫秒级延时函数（使用SysTick定时器）
void delay_ms(u16 nms)
{
    u16 temp;
    SysTick->VAL=0x00;              // 清空当前计数值
    SysTick->LOAD=900;              // 设置重装载值（9MHz时钟：900对应1ms）
    SysTick->CTRL=0x01;             // 使能SysTick（不使用中断）
    
    do
        temp=SysTick->CTRL;         // 读取控制寄存器状态
    while((temp&0x01)&&(!temp&(1<<16)));  // 等待计数完成
    
    SysTick->VAL=0x00;              // 清空计数值
    SysTick->CTRL=0x00;             // 关闭SysTick
}
