#include "key.h"
#include "delay.h" // 引入延时函数用于软件消抖

/**
 * @brief  初始化按键 GPIO (PB0)
 */
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 使能 GPIOB 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 2. 配置 PB0 为上拉输入 (因为外部按键一端接地，按下时为低电平)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief  按键扫描函数 (带软件消抖)
 * @param  mode: 0-不支持连续按(单次触发), 1-支持连续按
 * @retval 0-无按键按下, KEY1_PRES-按键1按下
 */
u8 KEY_Scan(u8 mode)
{	 
    static u8 key_up = 1; // 按键松开标志 (静态变量，保持上一次扫描的状态)
    
    if(mode) key_up = 1;  // 支持连按时，每次都清除松开标志
    
    // 如果之前按键是松开的，且当前检测到低电平 (可能被按下)
    if(key_up && (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0))
    {
        delay_ms(10); // 核心：软件消抖延时 10ms，避开机械抖动期
        
        key_up = 0;   // 标记按键已被按下
        
        // 延时后再次确认引脚状态，如果仍为低电平，说明是真正的按下动作
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
        {
            return KEY1_PRES; 
        }
    }
    // 如果检测到引脚为高电平，说明按键已松开
    else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1)
    {
        key_up = 1; 
    }
    
    return 0; // 无有效按键动作
}
