#ifndef _KEY_H
#define _KEY_H
#include "stm32f10x.h"

/* 按键值定义 */
#define KEY1_PRES    1      /* KEY1按下值 */
#define KEY2_PRES    2      /* 预留扩展按键 */
#define KEY3_PRES    3      /* 预留扩展按键 */

/* 函数声明 */
void KEY_Init(void);
u8 KEY_Scan(u8 mode);         /* 按键扫描函数声明 */

#endif
