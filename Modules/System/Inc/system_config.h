/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 */
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

// 引脚定义
// 传感器引脚（Port A）
// 注意：土壤和光敏必须接 3.3V，绝对不能接 5V！
extern ADC_HandleTypeDef hadc1;
#define ADC_SENSOR_HANDLE            (&hadc1)
#define SOIL_MOISTURE_ADC_CHANNEL    ADC_CHANNEL_0   // PA0
#define LIGHT_SENSOR_ADC_CHANNEL     ADC_CHANNEL_1   // PA1
#define DHT11_PORT                   GPIOB
#define DHT11_PIN                    GPIO_PIN_12     // PB12
// 通信引脚（PPort A）
// 蓝牙（USART1）
#define BLUETOOTH_UART               USART1
#define BLUETOOTH_TX_PORT            GPIOA
#define BLUETOOTH_TX_PIN             GPIO_PIN_9      // PA9
#define BLUETOOTH_RX_PORT            GPIOA
#define BLUETOOTH_RX_PIN             GPIO_PIN_10     // PA10
// 执行器引脚（Port B）
// 继电器全部移至 Port B，减少对 ADC 的干扰
#define RELAY_PUMP_PORT              GPIOB
#define RELAY_PUMP_PIN               GPIO_PIN_13     // PB13
#define RELAY_FAN_PORT               GPIOB
#define RELAY_FAN_PIN                GPIO_PIN_14     // PB14
// 风扇 PWM
#define FAN_PWM_PORT                 GPIOB
#define FAN_PWM_PIN                  GPIO_PIN_0      // PB0
#define FAN_PWM_TIM                  TIM3
#define FAN_PWM_CHANNEL              TIM_CHANNEL_3
// 系统引脚
// 板载 LED（PC13），低电平点亮
#define SYSTEM_LED_PORT              GPIOC
#define SYSTEM_LED_PIN               GPIO_PIN_13
// WiFi 预留 / 调试（USART2）
#define WIFI_UART                    USART2
#define WIFI_TX_PIN                  GPIO_PIN_2      // PA2
#define WIFI_RX_PIN                  GPIO_PIN_3      // PA3

// 默认阈值配置
#define DEFAULT_SOIL_MOISTURE_LOW    30.0f   // 土壤湿度低于 30% 开启水泵
#define DEFAULT_SOIL_MOISTURE_HIGH   40.0f   // 土壤湿度高于 40% 关闭水泵
#define DEFAULT_TEMP_HIGH            30.0f   // 温度高于 30°C 开启风扇
#define DEFAULT_TEMP_LOW             25.0f   // 温度低于 25°C 关闭风扇

// 系统参数
#define SYSTEM_TICK_MS               1000    // 主循环周期
#define HYSTERESIS_BAND              2.0f    // 滞回带宽
#define MAX_RETRY_COUNT              3       // 传感器重试次数

#endif // SYSTEM_CONFIG_H
