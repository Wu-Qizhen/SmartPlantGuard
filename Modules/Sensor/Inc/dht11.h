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

// DHT11 初始化
SensorStatusEnum DHT11_Init(GPIO_TypeDef *port, uint16_t pin);

// 读取温湿度数据
SensorStatusEnum DHT11_Read(float *temperature, float *humidity);

// 获取最后一次读取的状态
SensorStatusEnum DHT11_GetLastStatus(void);

// 获取读取统计
void DHT11_GetStatistics(uint32_t *successCount, uint32_t *errorCount);

#endif // DHT11_H
