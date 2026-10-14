/**
  ******************************************************************************
  * @file    bh1750.c
  * @brief   BH1750 光照传感器驱动 (包含软件模拟 I2C 与安全调光逻辑)
  ******************************************************************************
  */
#include "bh1750.h"
#include "delay.h"
#include "timer.h"
#include <stdio.h>

/* ================== 软件 I2C 引脚宏定义 (PB6, PB7) ================== */
#define BH_SCL_PORT     GPIOB
#define BH_SCL_PIN      GPIO_Pin_6
#define BH_SDA_PORT     GPIOB
#define BH_SDA_PIN      GPIO_Pin_7

// 使用位带操作或寄存器操作快速翻转引脚 (此处使用库函数以保证兼容性)
#define IIC_SCL_HIGH()  GPIO_SetBits(BH_SCL_PORT, BH_SCL_PIN)
#define IIC_SCL_LOW()   GPIO_ResetBits(BH_SCL_PORT, BH_SCL_PIN)

#define IIC_SDA_HIGH()  GPIO_SetBits(BH_SDA_PORT, BH_SDA_PIN)
#define IIC_SDA_LOW()   GPIO_ResetBits(BH_SDA_PORT, BH_SDA_PIN)
#define IIC_SDA_READ()  GPIO_ReadInputDataBit(BH_SDA_PORT, BH_SDA_PIN)

/* ================== 软件 I2C 底层驱动 ================== */

// 初始化 I2C 引脚为开漏输出模式 (开漏模式下既可输出也可读取外部电平)
static void IIC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = BH_SCL_PIN | BH_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出 (重要)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    IIC_SCL_HIGH();
    IIC_SDA_HIGH();
}

static void IIC_Start(void) {
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
    delay_us(4);
    IIC_SDA_LOW(); // SCL 高电平时，SDA 由高变低表示起始信号
    delay_us(4);
    IIC_SCL_LOW(); // 钳住 I2C 总线，准备发送或接收数据
}

static void IIC_Stop(void) {
    IIC_SCL_LOW();
    IIC_SDA_LOW();
    delay_us(4);
    IIC_SCL_HIGH();
    IIC_SDA_HIGH(); // SCL 高电平时，SDA 由低变高表示停止信号
    delay_us(4);
}

// 产生 ACK 应答
static void IIC_Ack(void) {
    IIC_SCL_LOW();
    IIC_SDA_LOW();
    delay_us(2);
    IIC_SCL_HIGH();
    delay_us(2);
    IIC_SCL_LOW();
}

// 不产生 ACK 应答
static void IIC_NAck(void) {
    IIC_SCL_LOW();
    IIC_SDA_HIGH();
    delay_us(2);
    IIC_SCL_HIGH();
    delay_us(2);
    IIC_SCL_LOW();
}

// 等待应答信号 (包含异常处理 1：超时退出，防止系统死锁)
static u8 IIC_Wait_Ack(void) {
    u8 errTime = 0;
    IIC_SCL_LOW();
    IIC_SDA_HIGH(); delay_us(1); // 释放 SDA 线
    IIC_SCL_HIGH(); delay_us(1);
    
    while(IIC_SDA_READ()) {
        errTime++;
        if(errTime > 250) {
            IIC_Stop();
            return 1; // 接收应答失败 (传感器未连接或出错)
        }
    }
    IIC_SCL_LOW();
    return 0; // 接收应答成功
}

// 发送一个字节
static void IIC_Send_Byte(u8 txd) {                        
    u8 t;   
    IIC_SCL_LOW();  // 拉低时钟开始数据传输
    for(t=0; t<8; t++) {              
        if((txd & 0x80) >> 7) IIC_SDA_HIGH();
        else IIC_SDA_LOW();
        txd <<= 1;       
        delay_us(2);   
        IIC_SCL_HIGH();
        delay_us(2); 
        IIC_SCL_LOW();  
        delay_us(2);
    }    
}

// 读一个字节，ack=1 发送 ACK，ack=0 发送 nACK 
static u8 IIC_Read_Byte(u8 ack) {
    u8 i, receive = 0;
    IIC_SDA_HIGH(); // 释放总线，准备读取
    for(i=0; i<8; i++) {
        IIC_SCL_LOW(); 
        delay_us(2);
        IIC_SCL_HIGH();
        receive <<= 1;
        if(IIC_SDA_READ()) receive++;   
        delay_us(1); 
    }                    
    if (!ack) IIC_NAck(); else IIC_Ack(); 
    return receive;
}


/* ================== BH1750 核心应用层驱动 ================== */

/**
 * @brief  初始化 BH1750
 */
void BH1750_Init(void) {
    IIC_Init();
    
    // 发送通电指令 (0x01)
    IIC_Start();
    IIC_Send_Byte(0x46); // BH1750 写地址 (ADDR接地时)
    if(IIC_Wait_Ack() != 0) {
        printf("BH1750 初始化失败: 未检测到传感器!\r\n");
        return;
    }
    IIC_Send_Byte(0x01); // Power On
    IIC_Wait_Ack();
    IIC_Stop();
}

/**
 * @brief  安全读取光照度数值 (包含超时和异常过滤机制)
 * @retval 当前光照度 Lux (若返回 -1.0 表示读取失败)
 */
float BH1750_ReadLux_Safe(void) {
    u8 buf[2] = {0};
    float lux = 0;

    // 1. 发送测量指令: 连续高分辨率模式 (0x10)
    IIC_Start();
    IIC_Send_Byte(0x46); // 写地址
    if(IIC_Wait_Ack()) return -1.0; 
    IIC_Send_Byte(0x10); 
    if(IIC_Wait_Ack()) return -1.0;
    IIC_Stop();
    
    // 2. 等待测量完成 (高分辨率模式最大需要 180ms)
    delay_ms(180);
    
    // 3. 读取测量数据
    IIC_Start();
    IIC_Send_Byte(0x47); // 读地址
    if(IIC_Wait_Ack()) return -1.0; 
    
    buf[0] = IIC_Read_Byte(1); // 读取高 8 位并发送 ACK
    buf[1] = IIC_Read_Byte(0); // 读取低 8 位并发送 NACK
    IIC_Stop();
    
    // 4. 数据换算 (原始数据 / 1.2 = Lux)
    lux = (float)(((u16)buf[0] << 8) + buf[1]) / 1.2;
    
    return lux;
}
/**
 * @brief  带有异常处理的安全调光逻辑
 * @param  lux: 当前测量到的环境光强
 */
void Auto_Dimming_Logic_Safe(float lux) {
    u8 target_duty = 0;
    
    // 异常处理 2：传感器读值异常或断线 (Failsafe 模式)
    if(lux < 0) {
        // 安全模式：如果硬件出错，将 LED 固定为 50% 亮度，保证基础照明
        printf("警告：传感器异常，进入安全调光模式(50%%)\r\n");
        LED_Set_PWM_Duty(50); // 注意：此处需调用 timer.c 中的驱动函数
        return; 
    }
    
    // 正常调光逻辑：光线越暗，灯越亮
    if(lux > 1000) {
        target_duty = 0;   // 环境非常亮，关灯
    }
    else if(lux < 10) {
        target_duty = 100; // 环境非常暗，全功率亮起
    }
    else {
        // 10 ~ 1000 Lux 区间内线性调光
        target_duty = 100 - (u8)((lux / 1000.0) * 100);
    }
    
    // 执行调光
    LED_Set_PWM_Duty(target_duty);
}