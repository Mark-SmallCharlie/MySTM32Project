/**
  ******************************************************************************
  * @file    bh1750.c
  * @brief   BH1750 ���մ��������� (��������ģ�� I2C �밲ȫ�����߼�)
  ******************************************************************************
  */
#include "bh1750.h"
#include "delay.h"
#include "timer.h"
#include <stdio.h>

/* ================== ���� I2C ���ź궨�� (PB6, PB7) ================== */
#define BH_SCL_PORT     GPIOB
#define BH_SCL_PIN      GPIO_Pin_6
#define BH_SDA_PORT     GPIOB
#define BH_SDA_PIN      GPIO_Pin_7

// ʹ��λ��������Ĵ����������ٷ�ת���� (�˴�ʹ�ÿ⺯���Ա�֤������)
#define IIC_SCL_HIGH()  GPIO_SetBits(BH_SCL_PORT, BH_SCL_PIN)
#define IIC_SCL_LOW()   GPIO_ResetBits(BH_SCL_PORT, BH_SCL_PIN)

#define IIC_SDA_HIGH()  GPIO_SetBits(BH_SDA_PORT, BH_SDA_PIN)
#define IIC_SDA_LOW()   GPIO_ResetBits(BH_SDA_PORT, BH_SDA_PIN)
#define IIC_SDA_READ()  GPIO_ReadInputDataBit(BH_SDA_PORT, BH_SDA_PIN)

/* ================== ���� I2C �ײ����� ================== */

// ��ʼ�� I2C ����Ϊ��©���ģʽ (��©ģʽ�¼ȿ����Ҳ�ɶ�ȡ�ⲿ��ƽ)
static void IIC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = BH_SCL_PIN | BH_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // ��©��� (��Ҫ)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    IIC_SCL_HIGH();
    IIC_SDA_HIGH();
}

static void IIC_Start(void) {
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
    delay_us(4);
    IIC_SDA_LOW(); // SCL �ߵ�ƽʱ��SDA �ɸ߱�ͱ�ʾ��ʼ�ź�
    delay_us(4);
    IIC_SCL_LOW(); // ǯס I2C ���ߣ�׼�����ͻ��������
}

static void IIC_Stop(void) {
    IIC_SCL_LOW();
    IIC_SDA_LOW();
    delay_us(4);
    IIC_SCL_HIGH();
    IIC_SDA_HIGH(); // SCL �ߵ�ƽʱ��SDA �ɵͱ�߱�ʾֹͣ�ź�
    delay_us(4);
}

// ���� ACK Ӧ��
static void IIC_Ack(void) {
    IIC_SCL_LOW();
    IIC_SDA_LOW();
    delay_us(2);
    IIC_SCL_HIGH();
    delay_us(2);
    IIC_SCL_LOW();
}

// ������ ACK Ӧ��
static void IIC_NAck(void) {
    IIC_SCL_LOW();
    IIC_SDA_HIGH();
    delay_us(2);
    IIC_SCL_HIGH();
    delay_us(2);
    IIC_SCL_LOW();
}

// �ȴ�Ӧ���ź� (�����쳣���� 1����ʱ�˳�����ֹϵͳ����)
static u8 IIC_Wait_Ack(void) {
    u8 errTime = 0;
    IIC_SCL_LOW();
    IIC_SDA_HIGH(); delay_us(1); // �ͷ� SDA ��
    IIC_SCL_HIGH(); delay_us(1);
    
    while(IIC_SDA_READ()) {
        errTime++;
        if(errTime > 250) {
            IIC_Stop();
            return 1; // ����Ӧ��ʧ�� (������δ���ӻ����)
        }
    }
    IIC_SCL_LOW();
    return 0; // ����Ӧ��ɹ�
}

// ����һ���ֽ�
static void IIC_Send_Byte(u8 txd) {                        
    u8 t;   
    IIC_SCL_LOW();  // ����ʱ�ӿ�ʼ���ݴ���
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

// ��һ���ֽڣ�ack=1 ���� ACK��ack=0 ���� nACK 
static u8 IIC_Read_Byte(u8 ack) {
    u8 i, receive = 0;
    IIC_SDA_HIGH(); // �ͷ����ߣ�׼����ȡ
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


/* ================== BH1750 ����Ӧ�ò����� ================== */

/**
 * @brief  ��ʼ�� BH1750
 */
void BH1750_Init(void) {
    IIC_Init();
    
    // ����ͨ��ָ�� (0x01)
    IIC_Start();
    IIC_Send_Byte(0x46); // BH1750 д��ַ (ADDR�ӵ�ʱ)
    if(IIC_Wait_Ack() != 0) {
        printf("BH1750 ��ʼ��ʧ��: δ��⵽������!\r\n");
        return;
    }
    IIC_Send_Byte(0x01); // Power On
    IIC_Wait_Ack();
    IIC_Stop();
}

/**
 * @brief  ��ȫ��ȡ���ն���ֵ (������ʱ���쳣���˻���)
 * @retval ��ǰ���ն� Lux (������ -1.0 ��ʾ��ȡʧ��)
 */
float BH1750_ReadLux_Safe(void) {
    u8 buf[2] = {0};
    float lux = 0;

    // 1. ���Ͳ���ָ��: �����߷ֱ���ģʽ (0x10)
    IIC_Start();
    IIC_Send_Byte(0x46); // д��ַ
    if(IIC_Wait_Ack()) return -1.0; 
    IIC_Send_Byte(0x10); 
    if(IIC_Wait_Ack()) return -1.0;
    IIC_Stop();
    
    // 2. �ȴ�������� (�߷ֱ���ģʽ�����Ҫ 180ms)
    delay_ms(180);
    
    // 3. ��ȡ��������
    IIC_Start();
    IIC_Send_Byte(0x47); // ����ַ
    if(IIC_Wait_Ack()) return -1.0; 
    
    buf[0] = IIC_Read_Byte(1); // ��ȡ�� 8 λ������ ACK
    buf[1] = IIC_Read_Byte(0); // ��ȡ�� 8 λ������ NACK
    IIC_Stop();
    
    // 4. ���ݻ��� (ԭʼ���� / 1.2 = Lux)
    lux = (float)(((u16)buf[0] << 8) + buf[1]) / 1.2;
    
    return lux;
}
/**
 * @brief  �����쳣�����İ�ȫ�����߼�
 * @param  lux: ��ǰ�������Ļ�����ǿ
 */
void Auto_Dimming_Logic_Safe(float lux) {
    u8 target_duty = 0;
    
    // �쳣���� 2����������ֵ�쳣����� (Failsafe ģʽ)
    if(lux < 0) {
        // ��ȫģʽ�����Ӳ���������� LED �̶�Ϊ 50% ���ȣ���֤��������
        printf("���棺�������쳣�����밲ȫ����ģʽ(50%%)\r\n");
        LED_Set_PWM_Duty_Safe(50); // 使用安全版本的PWM设置函数
        return; 
    }
    
    // ���������߼�������Խ������Խ��
    if(lux > 1000) {
        target_duty = 0;   // �����ǳ������ص�
    }
    else if(lux < 10) {
        target_duty = 100; // �����ǳ�����ȫ��������
    }
    else {
        // 10 ~ 1000 Lux ���������Ե���
        target_duty = 100 - (u8)((lux / 1000.0) * 100);
    }
    
    // ִ�е���
    LED_Set_PWM_Duty_Safe(target_duty);
}