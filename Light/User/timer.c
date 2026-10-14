#include "timer.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/* --- Hardware Pin Definition (Do Not Modify) --- */
#define LED_PWM_TIM             TIM2
#define LED_PWM_TIM_CLK         RCC_APB1Periph_TIM2
#define LED_PWM_PORT            GPIOA
#define LED_PWM_PIN             GPIO_Pin_1
#define LED_PWM_PORT_CLK        RCC_APB2Periph_GPIOA

/* --- Function Declarations --- */
void TIM2_PWM_Init(u16 arr, u16 psc);         // Initialize TIM2 for PWM
void LED_Set_PWM_Duty(u8 duty_percent);       // Set LED brightness percentage (0~100)
// Static variable to record current duty cycle
static u8 current_duty = 0;

void LED_Set_PWM_Duty_Safe(u8 target_duty) {
    // Exception 1: Boundary clamping
    if(target_duty > 100) target_duty = 100;
    
    // Exception 2: Smooth transition, gradual approach
    // Prevent sudden brightness changes causing visual discomfort and current impact
    if (target_duty > current_duty) current_duty++;
    else if (target_duty < current_duty) current_duty--;
    
    u16 ccr_val = (u16)((current_duty / 100.0) * 900); 
    TIM_SetCompare2(TIM2, ccr_val);
}

void TIM2_PWM_Init(u16 arr, u16 psc) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA1 Ϊ TIM2_CH2 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Timer base unit configuration
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // PWM Mode 1 configuration
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_Cmd(TIM2, ENABLE);
}

void LED_Set_PWM_Duty(u8 duty_percent) {
    if(duty_percent > 100) duty_percent = 100;
    u16 ccr_val = (u16)((duty_percent / 100.0) * 900); 
    TIM_SetCompare2(TIM2, ccr_val);
}


