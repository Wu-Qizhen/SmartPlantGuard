/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef DHT11_H
#define DHT11_H

#include "delay.h"
#include <stddef.h>
#include "sensor_types.h"
#include "stm32f1xx_hal.h"

/**
 * DHT11 初始化
 * @param port GPIO 端口
 * @param pin GPIO 引脚
 * @return 初始化状态
 */
SensorStatusEnum DHT11_Init(GPIO_TypeDef *port, uint16_t pin);

/**
 * 读取温湿度数据
 * @param temperature 返回温度值
 * @param humidity 返回湿度值
 * @return 读取状态
 */
SensorStatusEnum DHT11_Read(float *temperature, float *humidity);

/**
 * 获取最后一次读取的状态
 * @return 最后一次读取的状态
 */
SensorStatusEnum DHT11_GetLastStatus(void);

/**
 * 获取读取统计
 * @param successCount 成功次数
 * @param errorCount 错误次数
 */
void DHT11_GetStatistics(uint32_t *successCount, uint32_t *errorCount);

#endif // DHT11_H
