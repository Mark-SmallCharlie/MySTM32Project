<div align="center">

# STM32 单片机技术基础 · 实验与作业合集

**STM32 Embedded Labs & Coursework Collection**

STM32F103 系列 · 标准外设库 (SPL) + HAL 库 · Keil MDK-ARM

![MCU](https://img.shields.io/badge/MCU-STM32F103-03234B?logo=stmicroelectronics&logoColor=white)
![Library](https://img.shields.io/badge/Library-SPL%20V3.5.0%20%7C%20HAL-1E90FF)
![IDE](https://img.shields.io/badge/IDE-Keil%20MDK--ARM%20V5-0091BD)
![Tool](https://img.shields.io/badge/Tool-STM32CubeMX%205.3.0-2C8EBB)
![Language](https://img.shields.io/badge/Language-C%20%7C%20ASM%20%7C%20Java-orange)

</div>

---

## 📖 项目简介

本仓库是《单片机技术基础》课程实验与大作业的完整工程归档，涵盖 **GPIO 输入输出、外部中断、定时器中断、PWM 输出、串口通信（同步/异步）、传感器驱动（BH1750 光照 / DHT11 温湿度）** 等核心知识点。

代码分为两条技术路线：

| 路线 | 库 | 开发方式 | 代表工程 |
| :--- | :--- | :--- | :--- |
| **标准外设库 (SPL)** | STM32 标准外设库 V3.5.0 | 手写寄存器级驱动，分层架构 | `Light`、`Timer` |
| **HAL 库** | STM32Cube FW_F1 V1.8.4 | STM32CubeMX 图形化配置 + MDK-ARM | `09` ~ `T13` 系列 |

---

## 🗂️ 目录总览

```text
兵哥的作业/
├── Light/                 # ⭐ 大作业：基于 STM32 的智能光照监测与 LED 调光系统
├── Light - 备份/           # Light 工程的完整备份（源码 + 编译产物）
├── Timer/                 # SPL 库综合驱动模板（DHT11 / 串口 / 按键 / 定时器）
├── 09 ~ T13/              # 11 个 HAL 库实验工程（CubeMX 生成，MDK-ARM 编译）
├── 开发文档/               # 物联网模块开发资料（ESP8266 / CC2530 / LoRa）
├── untitled/              # Java 空工程模板
├── 光照传感器readme.md      # 光照传感器大作业说明文档
└── README.md              # 本文件
```

---

## ⭐ 核心项目

### 1. Light — 智能光照监测与 LED 调光系统

基于 **STM32F103C8T6 + BH1750 数字光照传感器** 的嵌入式闭环控制系统，实现「环境光强采集 → 模式判断 → PWM 无级调光 → 串口回传」的完整链路。

**核心特性**

- 🌞 **高精度光照采集**：BH1750 数字传感器，量程 0–65535 Lux
- 💡 **平滑无级调光**：定时器高频 PWM 输出，步进消抖实现平滑过渡
- 🔄 **双模式切换**：自动模式（环境越暗 LED 越亮）/ 手动模式（固定亮度）
- 📡 **实时状态监控**：USART 串口 9600 8N1 上传光强、占空比与工作模式
- 🛡️ **故障安全设计**：I²C 总线超时防卡死，传感器断线自动维持 50% 基础照明

**引脚分配**

| 模块 | 型号 | 引脚 | 说明 |
| :--- | :--- | :--- | :--- |
| 主控 MCU | STM32F103C8T6 | — | 72 MHz 主频 |
| 光照传感器 | BH1750 | PB6 (SCL) / PB7 (SDA) | 软件模拟 I²C |
| LED 调光 | 大功率 LED | PA1 | TIM2_CH2 复用推挽 (PWM) |
| 状态指示灯 | 板载 LED | PC13 | 500 ms 心跳翻转 |
| 模式按键 | 独立按键 | PB0 | 上拉输入，10 ms 软件消抖 |
| 串口调试 | USB-TTL | PA9 (TX) / PA10 (RX) | 9600, 8N1 |

**工程结构**（STM32 标准外设库 V3.5.0，分层架构）

```text
Light/
├── ASM/                  # 启动文件
├── Library/              # ST 官方标准外设库（inc/ + src/）
├── System/               # 内核与系统时钟配置（core_cm3、system_stm32f10x）
├── STM32f10x_it/         # 中断服务函数
├── User/                 # BSP 驱动层
│   ├── bh1750.c/.h       # 软件 I²C 与传感器数据处理
│   ├── timer.c/.h        # TIM2 PWM 调光底层驱动
│   ├── usart.c/.h        # 串口通信与 printf 重定向
│   ├── led.c/.h          # 状态指示灯驱动
│   ├── key.c/.h          # 按键采集与消抖
│   ├── delay.c/.h        # SysTick 精确延时
│   ├── adc.c/.h          # ADC 采集驱动
│   └── dht11.c/.h        # 温湿度传感器驱动（扩展）
├── main.c                # 主循环与业务控制调度
└── STM32.uvprojx         # Keil MDK-ARM 工程文件
```

> 📄 详细说明见 [`光照传感器readme.md`](光照传感器readme.md)
> 📦 `Light - 备份/` 为该工程的完整备份（含 `Objects/` 编译产物与 `Listings/` 链接清单）

---

### 2. Timer — SPL 库综合驱动模板

基于标准外设库的驱动程序集合，已实现 **DHT11 温湿度采集、USART1 串口输出（`printf` 重定向，115200 bps）、按键与外部中断、定时器、LED 控制** 等模块驱动；`main.c` 目前为骨架代码（完成 GPIO/USART 初始化），业务逻辑待补充。

```text
Timer/
├── ASM/                  # 启动文件
├── Library/              # 标准外设库
├── System/               # 内核与时钟配置
├── STM32f10x_it/         # 中断服务函数
├── User/                 # delay / dht11 / exti / key / led / timer / usart 驱动
└── main.c                # 主程序骨架
```

---

## 🧪 HAL 库实验工程（CubeMX + MDK-ARM）

全部基于 **STM32F103VETx（LQFP100，72 MHz）**，由 STM32CubeMX 5.3.0 生成，固件包 STM32Cube FW_F1 V1.8.4。

| 工程 | 实验主题 | 关键外设 | 引脚分配 |
| :--- | :--- | :--- | :--- |
| **`09`** | 定时器中断驱动 LED 闪烁（0.5 s 亮 / 1.5 s 灭） | TIM6 | LED5–LED8 → PE3/PE2/PE1/PE0 |
| **`10`** | 定时器中断驱动 8 位流水灯 | TIM6 | LED1–LED8 → PE7…PE0 |
| **`T-1-06`** | 按键控制四路 LED 依次翻转 | EXTI13 | KEY2 → PD13；LED1–LED4 → PE7…PE4 |
| **`T-1-07`** | 按键计数触发 LED 分组点亮 | EXTI13 + TIM6 | KEY1 → PC13；LED1/LED2 → PE7/PE6 |
| **`T-1-08`** | 按键触发蜂鸣器鸣响 + LED 翻转 | EXTI13 | KEY1 → PC13；BUZZ → PA8；LED1 → PE7 |
| **`T-1-11`** | PWM 实现 LED 呼吸灯（1 kHz） | TIM4_CH3 | Breathe → PB8 |
| **`12`** | 按键启动定时器控制 LED 交替闪烁 | EXTI13 + TIM6 | KEY2 → PD13；LED3/LED4 → PE5/PE4 |
| **`T-1-12`** | 按键 + 定时器 LED 闪烁（1 s 周期） | EXTI15_10 + TIM6 | KEY2 → PD13；LED3/LED4 → PE5/PE4 |
| **`T13`** | 串口发送字符串（"Hello STM32!"） | USART1 (异步) | PA9 (TX) / PA10 (RX) |
| **`T-1-14`** | 串口接收指令控制 LED（'1' / '2'） | USART1 (异步) | PA9/PA10；LED1/LED2 → PE7/PE6 |
| **`T-1-15`** | 串口同步模式通信（115200 bps） | USART1 (同步) | PA8 (CK) / PA9 (TX) / PA10 (RX)；LED3/LED4 → PE5/PE4 |
| **`T01-05`** | 外部中断控制双 LED 翻转 | EXTI13 | KEY1 → PC13；LED5/LED6 → PE3/PE2 |

**统一工程结构**

```text
T-1-xx/
├── Drivers/
│   ├── CMSIS/                    # 内核与设备头文件
│   └── STM32F1xx_HAL_Driver/     # HAL 驱动源码与头文件
├── Inc/                          # main.h / gpio.h / tim.h / usart.h / it.h
├── Src/                          # main.c / gpio.c / tim.c / usart.c / it.c
├── MDK-ARM/                      # Keil 工程（.uvprojx）、RTE 配置与编译输出
└── T-1-xx.ioc                    # STM32CubeMX 图形化配置文件
```

> `09`、`10`、`12` 三个工程采用数字编号，其对应的 `.ioc` 文件名分别为 `09.ioc`、`T-1-10.ioc`、`T-1-12.ioc`。

---

## 📚 开发文档

`开发文档/` 收录物联网常用无线通信模块的官方手册与入门资料：

| 模块 | 文档 |
| :--- | :--- |
| **ESP8266** (Wi-Fi) | AT 指令集 018、AT 指令使用示例 v0.2、FAQ 文档 v0.2、入门教程、ESP8288 模块 AT 基本运用 |
| **CC2530 / CC2540** (ZigBee / BLE) | CC2530 中文数据手册完全版、cc2530 芯片手册（中文）、cc2530-datasheet、cc2540-datasheet、CC253x and CC2540 User's Guide、ZigBee2007 协议栈 API 函数使用说明、ZigBee 紫蜂技术及其应用、CC-Debugger 用户向导 |
| **LoRa SX127x** | LoRaSX127x 参考设计、SX1276/77/78/79 芯片手册、LoRa 规范 CLASS-A、LoRa 官方文件说明、LoRa 技术及终端分类 |

---

## 🛠️ 工具链与编译

| 项目 | 版本 / 说明 |
| :--- | :--- |
| 主控芯片 | STM32F103VETx (LQFP100) / STM32F103C8T6 |
| 系统主频 | 72 MHz（HSE 8 MHz × PLL9） |
| HAL 工程 IDE | Keil MDK-ARM V5（`MDK-ARM/*.uvprojx`） |
| SPL 工程 IDE | Keil MDK-ARM V5（`STM32.uvprojx`） |
| 配置工具 | STM32CubeMX 5.3.0（`.ioc` 文件可重新生成代码） |
| 下载调试 | ST-Link / J-Link（SWD：PA13 = SWDIO，PA14 = SWCLK） |

**编译步骤**

1. 用 Keil MDK-ARM 打开对应工程的 `.uvprojx` 文件；
2. 确认已安装 STM32F1xx 器件支持包（Device Family Pack）；
3. 执行 `Build`（F7）编译，生成 `Objects/*.hex` 或 `MDK-ARM/<name>/<name>.hex`；
4. 连接调试器，执行 `Download`（F8）烧录至开发板。

> 工程中的 `Objects/`、`Listings/`、`MDK-ARM/<name>/` 等目录为编译中间产物，可按需清理或加入 `.gitignore`。

---

## 📁 其他文件

| 路径 | 说明 |
| :--- | :--- |
| `untitled/` | IntelliJ IDEA Java 工程模板（`src/pack/XKY.java` 为空类，`.idea/` 为 IDE 配置） |
| `光照传感器readme.md` | 光照传感器大作业的项目说明（功能、引脚、目录结构） |
| `Light/.claude/settings.local.json` | 本地工具权限配置 |
| `25-26-02-22、23物联网工程本科《单片机技术基础》重修考试-大作业-*.doc` | 课程大作业原始文档（2 份） |

---

## 💡 快速上手

```text
① 想了解光照调光大作业  →  进入 Light/，阅读 光照传感器readme.md
② 想做基础实验（GPIO/中断/定时器/PWM/串口）  →  从 09、T01-05 开始按编号递进
③ 需要无线通信模块资料  →  查阅 开发文档/
④ 想基于 SPL 手写驱动  →  参考 Timer/ 与 Light/User/ 的分层驱动写法
```

---

<div align="center">

**📌 说明**：本仓库为课程学习与实验代码归档，各工程相互独立，可按需单独编译与烧录。

</div>
