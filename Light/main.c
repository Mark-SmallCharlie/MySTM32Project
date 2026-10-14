#include "delay.h"
#include "led.h"
#include "key.h"
#include <stdio.h>      // 标准输入输出库
#include "timer.h"
#include "usart.h"      // 串口通信库
#include "bh1750.h"     // BH1750光照传感器库

// System working mode definitions
#define MODE_AUTO   0
#define MODE_MANUAL 1

u8  Target_Duty = 0;       // LED目标占空比
u8  System_Mode = MODE_AUTO; // 系统工作模式 

// Redirect printf function to use fputc
int fputc(int ch, FILE *f)
{
    USART1->DR = (u8)ch;
    while((USART1->SR & 0x40) == 0); // Wait for transmission complete flag
    return ch;
}

int main(void)
{
    float current_lux = 0;  

    /* ================= 1. System Hardware Initialization ================= */
    // Note: delay_init() is removed because delay.c directly manipulates registers
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
    
    USART_Config();           // Initialize USART (function in usart.c)
    LED_Init();               
    KEY_Init();               
    TIM2_PWM_Init(899, 79);   
    BH1750_Init();            

    printf("--- Smart Lighting System Started ---\r\n");

    /* ================= 2. Main Loop ================= */
    while(1)
    {
        /* --- A. External Button Mode Switching --- */
        if(KEY_Scan(0) == KEY1_PRES) 
        {
            System_Mode = !System_Mode; 
            if(System_Mode == MODE_AUTO)
                printf("Switched: Auto Mode\r\n");
            else
                printf("Switched: Manual Mode\r\n");
        }

        /* --- B. 光照数据采集 --- */
        // 使用带安全控制的读取函数
        current_lux = BH1750_ReadLux_Safe();

        // 错误处理：传感器读取失败
        if(current_lux < 0) {
            printf("Warning: Light sensor reading failed!\r\n");
            // 进入安全模式，设置50%亮度
            LED_Set_PWM_Duty_Safe(50);
            continue; // 跳过本次循环的后续处理
        } 

        /* --- C. LED Dimming Logic Execution --- */
        if(System_Mode == MODE_AUTO)
        {
            // Use safe dimming control logic with security features
            Auto_Dimming_Logic_Safe(current_lux); 
        }
        else
        {
            // 手动模式逻辑：固定50%亮度
            // 注意：当前硬件只支持一个按键（模式切换键）
            // 如需手动模式亮度调节，需扩展硬件支持更多按键
            LED_Set_PWM_Duty_Safe(50); // 固定50%亮度
            Target_Duty = 50;
        }

        /* --- D. System Status Display Output --- */
        printf("Light: %.2f Lux | Mode: %s\r\n",
               current_lux,
               (System_Mode == MODE_AUTO) ? "Auto" : "Manual");
        
        LED_Toggle(); 

        delay_ms(500); 
    }
}