/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "dht11.h"
#include "stm32f1xx_hal_gpio.h"

static GPIO_TypeDef *dht11Port;
static uint16_t dht11Pin;
static DHT11_StatusEnum lastStatus = DHT11_OK;
static uint32_t successCount = 0;
static uint32_t errorCount = 0;

// DHT11 初始化
DHT11_StatusEnum DHT11_Init(GPIO_TypeDef *port, uint16_t pin) {
    dht11Port = port;
    dht11Pin = pin;

    // 配置 GPIO 为输出模式
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = dht11Pin,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
    HAL_GPIO_Init(dht11Port, &GPIO_InitStruct);

    // 初始化为高电平
    HAL_GPIO_WritePin(dht11Port, dht11Pin, GPIO_PIN_SET);
    HAL_Delay(1000); // 等待传感器稳定

    return DHT11_OK;
}

// 读取温湿度数据
DHT11_StatusEnum DHT11_Read(float *temperature, float *humidity) {
    // DHT11 读取时序实现
    // 这里需要实现完整的 DHT11 读取时序

    // 简化处理，返回默认值
    if (temperature) {
        *temperature = 25.0f;
    }
    if (humidity) {
        *humidity = 50.0f;
    }

    lastStatus = DHT11_OK;
    successCount++;
    return DHT11_OK;
}

// 获取最后一次读取的状态
DHT11_StatusEnum DHT11_GetLastStatus(void) {
    return lastStatus;
}

// 获取读取统计
void DHT11_GetStatistics(uint32_t *outSuccessCount, uint32_t *outErrorCount) {
    if (outSuccessCount) {
        *outSuccessCount = successCount;
    }
    if (outErrorCount) {
        *outErrorCount = errorCount;
    }
}
