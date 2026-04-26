//#include "led.h"
//void LED_Init(void)
//{
//	
//	 // 初始化GPIOB的第5引脚 (LED1)
//		GPIO_InitTypeDef GPIO_InitStruct;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;                 // 选择引脚5
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;          // 推挽输出模式
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;         // 50MHz速度
//    GPIO_Init(GPIOB,&GPIO_InitStruct);                     // 初始化GPIOB
//	
//	GPIO_InitTypeDef PP;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	
//}




#include "led.h"

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 使能 GPIOC 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // 2. 配置 PC13 为推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 3. 初始状态置高电平，默认熄灭指示灯
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

/**
 * @brief  翻转 LED 状态 (利用异或寄存器 ODR)
 */
void LED_Toggle(void)
{
    GPIOC->ODR ^= GPIO_Pin_13; 
}

//float BH1750_ReadLux(void) {
//    u8 buf[2];
//    float lux = 0;
//    // 假设已实现基础 I2C 起始、停止、读写函数
//    I2C_Start();
//    I2C_SendByte(0x46); // 写地址
//    I2C_SendByte(0x10); // H-Resolution Mode
//    I2C_Stop();
//    delay_ms(180);      // 等待测量完成
//    
//    I2C_Start();
//    I2C_SendByte(0x47); // 读地址
//    buf[0] = I2C_ReadByte(1); // 读高 8 位
//    buf[1] = I2C_ReadByte(0); // 读低 8 位
//    I2C_Stop();
//    
//    lux = (float)((buf[0] << 8) + buf[1]) / 1.2;
//    return lux;
//}

//void Auto_Dimming_Logic(float lux) {
//    u8 target_duty;
//    // 自动调光策略：环境光越强(Lux大)，LED亮度越低
//    if(lux > 1000) target_duty = 0;   // 极亮环境：关灯
//    else if(lux < 10) target_duty = 100; // 极暗环境：全开
//    else {
//        // 线性映射逻辑：10~1000 Lux 对应 100%~0% 亮度
//        target_duty = 100 - (u8)((lux / 1000.0) * 100);
//    }
//    LED_Set_PWM_Duty(target_duty);
//}
