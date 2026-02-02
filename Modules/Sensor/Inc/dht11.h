#ifndef DHT11_H
#define DHT11_H

#include "sensor_types.h"

// DHT11 特定错误码
typedef enum {
    DHT11_OK = 0,
    DHT11_NO_RESPONSE,
    DHT11_CHECKSUM_ERROR,
    DHT11_TIMEOUT_ERROR
} DHT11_StatusEnum;

// DHT11 初始化
DHT11_StatusEnum DHT11_Init(GPIO_TypeDef* port, uint16_t pin);

// 读取温湿度数据
DHT11_StatusEnum DHT11_Read(float* temperature, float* humidity);

// 获取最后一次读取的状态
DHT11_StatusEnum DHT11_GetLastStatus(void);

// 获取读取统计
void DHT11_GetStatistics(uint32_t* successCount, uint32_t* errorCount);

#endif // DHT11_H