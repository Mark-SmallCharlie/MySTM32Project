#include "usart.h"
#include "stm32f10x.h"
#include <string.h>

// 定义接收缓冲区
#define RX_BUFFER_SIZE 64
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint16_t rx_index = 0;
volatile uint8_t rx_complete = 0;


void RCC_Configuration(void)
{
    // 1. 复位 RCC 外设寄存器到默认状态
    RCC_DeInit();
    
    // 2. 使能 HSE (外部高速时钟，通常是 8MHz 晶振)
    RCC_HSEConfig(RCC_HSE_ON);
    
    // 3. 等待 HSE 启动稳定
    if (RCC_WaitForHSEStartUp() == SUCCESS) {
        // 4. 配置 AHB, APB1, APB2 的预分频器
        RCC_HCLKConfig(RCC_SYSCLK_Div1);   // AHB 时钟 = SYSCLK (72MHz)
        RCC_PCLK2Config(RCC_HCLK_Div1);    // APB2 时钟 = AHB 时钟 (72MHz)
        RCC_PCLK1Config(RCC_HCLK_Div2);    // APB1 时钟 = AHB 时钟 / 2 (36MHz，因为最大36MHz)
        
        // 5. 配置 PLL (锁相环)：输入 8MHz，9倍频 -> 输出 72MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        
        // 6. 使能 PLL
        RCC_PLLCmd(ENABLE);
        
        // 7. 等待 PLL 准备就绪
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        
        // 8. 选择 PLL 作为系统时钟源 (SYSCLK)
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        // 9. 等待系统时钟源切换成功
        while (RCC_GetSYSCLKSource() != 0x08);
    }
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 开启 GPIOA, GPIOC 和复用功能(AFIO)的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

    // 【第 1 组：GPIOA - 模拟输入】用于 ADC 采集光敏电阻分压 (PA0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式，关闭数字缓冲，防止信号干扰
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 【第 2 组：GPIOA - 复用推挽输出】用于 TIM2 PWM 输出 (PA1) 和 串口 TX (PA9)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出，控制权交给内部外设(定时器和串口)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 输出翻转速度最高
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 串口 RX (PA10) 配置为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 【第 3 组：GPIOC - 普通推挽输出】用于板载状态指示灯 (PC13)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 普通推挽输出，由 CPU 直接控制电平
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 初始化时将 PC13 置高（若 LED 为低电平点亮，则初始状态为灭）
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}



void USART_Config(void)
{
    USART_InitTypeDef USART_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;                    // 定义GPIO结构体变量
    NVIC_InitTypeDef NVIC_InitStructure;                 // 定义NVIC结构体
    
    // 时钟使能配置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); // 使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);   // 使能AFIO时钟
    
    // TX引脚配置（PA9）- 复用推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;          // 复用推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    // RX引脚配置（PA10）- 浮空输入
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;    // 浮空输入
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    // USART1参数配置
    USART_InitStruct.USART_BaudRate=115200;              // 波特率115200
    USART_InitStruct.USART_WordLength=USART_WordLength_8b;  // 8位数据位
    USART_InitStruct.USART_Parity=USART_Parity_No;       // 无校验位
    USART_InitStruct.USART_StopBits=USART_StopBits_1;    // 1位停止位
    USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;  // 收发模式
    USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;  // 无硬件流控    
    USART_Init(USART1,&USART_InitStruct);                // 初始化USART1
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);         // 使能接收中断
    USART_Cmd(USART1,ENABLE);                            // 使能USART1
    USART_ClearFlag(USART1,USART_FLAG_TC);               // 清除发送完成标志
		
}

// NVIC中断配置
void NVICx_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);      // 设置优先级分组
    
    NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;         // USART1中断通道
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1; // 抢占优先级1
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=5;        // 子优先级5
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;           // 使能中断
    NVIC_Init(&NVIC_InitStruct);                         // 初始化NVIC
}

// 串口发送单个字符
void USART_SendChar(uint8_t ch)
{
    USART_SendData(USART1, ch);                          // 发送数据
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  // 等待发送完成
}

// 串口发送字符串
void USART_SendString(uint8_t *str)
{
    while(*str != '\0')
    {
        USART_SendChar(*str);                            // 逐个发送字符
        str++;
    }
}

// 串口中断服务函数
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  // 接收中断
    {
        uint8_t received_char = USART_ReceiveData(USART1);  // 读取接收到的字符
        
        // 处理接收到的字符
        if(received_char == '\r' || received_char == '\n')  // 回车或换行
        {
            if(rx_index > 0)  // 缓冲区有数据
            {
                rx_buffer[rx_index] = '\0';              // 添加字符串结束符
                rx_complete = 1;                         // 设置接收完成标志
                rx_index = 0;                            // 重置索引
                
                // 回显接收到的内容
                USART_SendString("\r\nYou typed: ");
                USART_SendString(rx_buffer);
                USART_SendString("\r\n> ");
            }
            else
            {
                USART_SendString("\r\n> ");
            }
        }
        else if(received_char == 0x08 || received_char == 0x7F)  // 退格键
        {
            if(rx_index > 0)
            {
                rx_index--;
                USART_SendString("\b \b");               // 回显退格效果
            }
        }
        else  // 普通字符
        {
            if(rx_index < RX_BUFFER_SIZE - 1)
            {
                rx_buffer[rx_index++] = received_char;   // 存入缓冲区
                USART_SendChar(received_char);           // 回显字符
            }
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // 清除中断标志
    }
}

// 用户调用函数
void USART_user(void)
{
    // 初始化接收缓冲区
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    rx_index = 0;
    rx_complete = 0;
    
    USART_Config();
    NVICx_Init();
    
    // 发送欢迎信息和提示符
    USART_SendString("\r\nSTM32 Serial Echo Demo\r\n");
    USART_SendString("Type something and press Enter:\r\n");
    USART_SendString("> ");
}

// 检查是否收到完整的数据
uint8_t USART_IsDataReceived(void)
{
    return rx_complete;
}

// 获取接收到的数据
uint8_t* USART_GetReceivedData(void)
{
    rx_complete = 0;  // 清除标志
    return rx_buffer;
}
