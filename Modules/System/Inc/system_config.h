/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 */
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "stm32f1xx_hal.h"

// 传感器引脚
extern ADC_HandleTypeDef hadc1;
#define ADC_SENSOR_HANDLE           (&hadc1)
#define SOIL_MOISTURE_ADC_CHANNEL   ADC_CHANNEL_0    // PA0
#define LIGHT_SENSOR_ADC_CHANNEL    ADC_CHANNEL_1    // PA1
#define DHT11_PORT                  GPIOB
#define DHT11_PIN                   GPIO_PIN_12      // PB12

// 通信引脚
#define BLUETOOTH_UART              USART1
#define BLUETOOTH_TX_PORT           GPIOA
#define BLUETOOTH_TX_PIN            GPIO_PIN_9      // PA9 -> BT24 RX
#define BLUETOOTH_RX_PORT           GPIOA
#define BLUETOOTH_RX_PIN            GPIO_PIN_10     // PA10 <- BT24 TX
#define BLUETOOTH_EN_PORT           GPIOB
#define BLUETOOTH_EN_PIN            GPIO_PIN_8      // PB8
#define BLUETOOTH_STATE_PORT        GPIOB
#define BLUETOOTH_STATE_PIN         GPIO_PIN_9      // PB9

// 存储引脚
#define FLASH_SPI_HANDLE            (&hspi1)
#define FLASH_CS_PORT               GPIOA
#define FLASH_CS_PIN                GPIO_PIN_4      // PA4 -> W25Q64 CS
#define FLASH_SCK_PORT              GPIOA
#define FLASH_SCK_PIN               GPIO_PIN_5      // PA5 -> W25Q64 SCK
#define FLASH_MISO_PORT             GPIOA
#define FLASH_MISO_PIN              GPIO_PIN_6      // PA6 -> W25Q64 MISO
#define FLASH_MOSI_PORT             GPIOA
#define FLASH_MOSI_PIN              GPIO_PIN_7      // PA7 -> W25Q64 MOSI

// 执行器引脚
// 继电器（移至 Port B，减少对 ADC 的干扰）
#define RELAY_PUMP_PORT             GPIOB
#define RELAY_PUMP_PIN              GPIO_PIN_13     // PB13
#define RELAY_LIGHT_PORT            GPIOB
#define RELAY_LIGHT_PIN             GPIO_PIN_14     // PB14
// 风扇
#define FAN_PWM_PORT                GPIOB
#define FAN_PWM_PIN                 GPIO_PIN_0      // PB0
#define FAN_PWM_TIM                 TIM3
#define FAN_PWM_CHANNEL             TIM_CHANNEL_3
#define FAN_AIN1_PORT               GPIOB
#define FAN_AIN1_PIN                GPIO_PIN_1      // PB1
#define FAN_AIN2_PORT               GPIOB
#define FAN_AIN2_PIN                GPIO_PIN_5      // PB5

// 系统引脚
// 板载 LED（低电平点亮）
#define SYSTEM_LED_PORT             GPIOC
#define SYSTEM_LED_PIN              GPIO_PIN_13     // PC13
// WiFi 预留 / 调试（USART2）
#define WIFI_UART                   USART2
#define WIFI_TX_PIN                 GPIO_PIN_2      // PA2
#define WIFI_RX_PIN                 GPIO_PIN_3      // PA3

// 默认阈值配置
#define DEFAULT_SOIL_MOISTURE_LOW        40.0f      // 土壤湿度低于开启水泵
#define DEFAULT_SOIL_MOISTURE_HIGH       60.0f      // 土壤湿度高于关闭水泵
#define DEFAULT_TEMP_HIGH                30.0f      // 温度高于开启风扇
#define DEFAULT_TEMP_LOW                 28.0f      // 温度低于关闭风扇
#define DEFAULT_LIGHT_INTENSITY_LOW      500.0f     // 光照强度低于开启补光灯
#define DEFAULT_LIGHT_INTENSITY_HIGH     800.0f     // 光照强度高于关闭补光灯
#define DEFAULT_PUMP_MIN_INTERVAL        300        // 时间小于无法再次启动水泵
#define DEFAULT_PUMP_MAX_DURATION        20         // 运行时间大于自动关闭水泵

// 默认校准参数配置
#define DEFAULT_SOIL_DRY_VALUE      4095.0f
#define DEFAULT_SOIL_WET_VALUE      0.0f
#define DEFAULT_LIGHT_MIN_ADC       0.0f
#define DEFAULT_LIGHT_MAX_ADC       4095.0f
#define DEFAULT_LIGHT_MIN_LUX       0.0f
#define DEFAULT_LIGHT_MAX_LUX       1500.0f

// 系统参数
#define SYSTEM_TICK_MS              1000            // 主循环周期
#define PWM_ARR_PERIOD              999             // PWM 周期
#define UART_BAUD_RATE              9600            // 串口波特率
#define SENSOR_READ_INTERVAL_MS     3000            // 传感器读取间隔
#define AUTO_CONTROL_INTERVAL_MS    5000            // 传感器读取间隔
#define MAX_DUTY_CYCLE              1000            // 最大占空比

#endif // SYSTEM_CONFIG_H
