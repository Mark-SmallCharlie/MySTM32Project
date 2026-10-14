#include "adc.h"
#include "delay.h" // 需要用到微小的延时来进行多次采样

/**
 * @brief  初始化 ADC1 和对应的 GPIO (PA0)
 * @param  无
 * @retval 无
 */
void Adc_Init(void)
{ 	
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 开启 GPIOA 和 ADC1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);	

    // 2. 设置 ADC 时钟分频因子为 6 (72M/6 = 12M)
    // 注意：ADC 最大时钟不能超过 14M，所以必须分频
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   

    // 3. 配置 PA0 为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		// 模拟输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

    // 4. 复位 ADC1，将外设 ADC1 的全部寄存器重设为默认值
    ADC_DeInit(ADC1);

    // 5. 初始化 ADC1 参数
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // 独立工作模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       // 单通道模式，关闭扫描
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                 // 单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发转换，不由外部中断触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                             // 顺序进行规则转换的 ADC 通道的数目为1
    ADC_Init(ADC1, &ADC_InitStructure);

    // 6. 使能指定的 ADC1
    ADC_Cmd(ADC1, ENABLE);

    // 7. ADC 校准 (必须执行，保证精度)
    ADC_ResetCalibration(ADC1);	                          // 使能复位校准
    while(ADC_GetResetCalibrationStatus(ADC1));           // 等待复位校准结束
    ADC_StartCalibration(ADC1);	                          // 开启 AD 校准
    while(ADC_GetCalibrationStatus(ADC1));                // 等待校准结束
}

/**
 * @brief  获取指定通道的 ADC 转换值
 * @param  ch: ADC 通道号 (例如 ADC_Channel_0)
 * @retval 12位的 ADC 值 (0~4095)
 */
u16 Get_Adc(u8 ch)   
{
    // 配置当前要转换的通道、转换顺序和采样时间 (239.5个周期，采样时间越长越稳定)
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	  			    
  
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);               // 使能指定的 ADC1 的软件转换启动功能	

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));        // 等待转换结束 (EOC标志位变为1)

    return ADC_GetConversionValue(ADC1);                  // 返回最近一次 ADC1 规则组的转换结果
}

/**
 * @brief  获取指定通道多次采样的平均值
 * @param  ch: ADC 通道号
 * @param  times: 采样次数
 * @retval 滤波后的 ADC 平均值
 */
u16 Get_Adc_Average(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;
    
    for(t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch); // 累加每次采样的值
        delay_ms(5);             // 延时5ms再采下一次，避免读取瞬时干扰尖峰
    }
    
    return (u16)(temp_val / times); // 返回平均值
}
