# STM32 Cube MX 配置清单

> 根据接口文档进行的配置

| 引脚 | 功能       | 外设      | 状态 | 备注                  |
| :--- | :--------- | :-------- | :--- | :-------------------- |
| PA0  | 土壤湿度   | ADC1_IN0  | ✅    | 必须接 3.3V           |
| PA1  | 光敏传感器 | ADC1_IN1  | ✅    | 必须接 3.3V           |
| PA9  | 蓝牙 TX    | USART1_TX | ✅    |                       |
| PA10 | 蓝牙 RX    | USART1_RX | ✅    |                       |
| PB0  | 风扇 PWM   | TIM3_CH3  | ✅    | 注意复用功能          |
| PB12 | DHT11 数据 | GPIO      | ⚠️    | 需外部上拉，3.3V 电平 |
| PB13 | 水泵继电器 | GPIO      | ✅    |                       |
| PB14 | 风扇继电器 | GPIO      | ✅    |                       |
| PC13 | 系统 LED   | GPIO      | ✅    | 低电平点亮            |
| PA13 | SWDIO      | 调试      | ✅    | 保留                  |
| PA14 | SWCLK      | 调试      | ✅    | 保留                  |

![](./IMG/STM32 Cube MX 配置清单.png)



## 1. 时钟树

```
HSE：8 MHz
PLL Source Mux：HSE
PLLMUL：×9
系统时钟源选择：PLLCLK
系统时钟（SYSCLK）设置到最大值：72 MHz
HCLK：72 MHz
PCLK1：36 MHz（APB1 总线时钟）
PCLK2：72 MHz（APB2 总线时钟）

最终配置：
- SYSCLK：72 MHz
- HCLK：72 MHz
- PCLK1：36 MHz
- PCLK2：72 MHz
```



## 2. 系统核心

- SYS：Debug → Serial Wire
- RCC：HSE → Crystal/Ceramic Resonator



## 3. GPIO

- PB12：GPIO_Output, Open Drain, Pull-up, Medium speed, High level（DHT11 数据线）
- PB13：GPIO_Output, Push Pull, No pull, Low speed, Low level（继电器控制引脚）
- PB14：GPIO_Output, Push Pull, No pull, Low speed, Low level（继电器控制引脚）
- PC13：GPIO_Output, Push Pull, Pull-up, Low speed, High level（系统 LED）



## 4. 定时器（风扇PWM）

- TIM3：Channel3 → PWM Generation CH3
- 参数：Prescaler=71, Counter Period=999



## 5. 串口（蓝牙 HC-05）

- USART1：Asynchronous
- 波特率：9600



## 6. ADC（土壤湿度和光敏传感器）

- ADC1：IN0 和 IN1 启用
- Scan Conversion Mode：Enabled
- Continuous Conversion Mode：Enabled
- DMA Continuous Requests：Enabled
- Number Of Conversion：2
- Rank1：Channel 0, 239.5 Cycles
- Rank2：Channel 1, 239.5 Cycles



## 7. DMA（ADC 数据传输）

- ADC1：一个DMA请求
- Mode：Circular
- Data Width：Half Word (Both Peripheral and Memory)
- Memory Increment Address：Enable



## 8. 项目管理

1. Project：
   - Project Name：SmartPlantGuard
   - Toolchain/IDE：CMake
2. Code Generator：
   - ✅ Generate peripheral initialization as a pair of '.c/.h' files per peripheral