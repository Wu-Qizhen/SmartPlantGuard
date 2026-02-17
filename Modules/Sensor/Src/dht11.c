/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "dht11.h"

static GPIO_TypeDef *dht11Port = NULL;
static uint16_t dht11Pin = 0;
static float lastTemperature = 25.0f;
static float lastHumidity = 50.0f;
static SensorStatusEnum lastStatus = SENSOR_OK;
static uint32_t successCount = 0;
static uint32_t errorCount = 0;

// 设置 GPIO 为推挽输出模式
static void DHT11_SetOutputMode(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht11Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(dht11Port, &GPIO_InitStruct);
}

// 设置 GPIO 为浮空输入模式
static void DHT11_SetInputMode(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dht11Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(dht11Port, &GPIO_InitStruct);
}

// 发送起始信号并等待传感器响应
static SensorStatusEnum DHT11_StartSignal(void) {
    // 主机拉低总线至少 18ms
    DHT11_SetOutputMode();
    HAL_GPIO_WritePin(dht11Port, dht11Pin, GPIO_PIN_RESET);
    HAL_Delay(20);

    // 拉高总线 30us
    HAL_GPIO_WritePin(dht11Port, dht11Pin, GPIO_PIN_SET);
    delay_us(30);

    // 切换为输入模式，准备接收响应
    DHT11_SetInputMode();
    delay_us(40);

    // 检查传感器是否拉低总线（响应信号）
    if (HAL_GPIO_ReadPin(dht11Port, dht11Pin) == GPIO_PIN_RESET) {
        // 等待拉低阶段结束（约 80us）
        uint32_t timeout = 0;
        while (HAL_GPIO_ReadPin(dht11Port, dht11Pin) == GPIO_PIN_RESET) {
            delay_us(1);
            if (++timeout > 100) return SENSOR_TIMEOUT;
        }
        // 等待拉高阶段结束（约 80us）
        timeout = 0;
        while (HAL_GPIO_ReadPin(dht11Port, dht11Pin) == GPIO_PIN_SET) {
            delay_us(1);
            if (++timeout > 100) return SENSOR_TIMEOUT;
        }
        return SENSOR_OK;
    }
    return SENSOR_TIMEOUT; // 原 DHT11_NO_RESPONSE
}

// 读取一个字节数据
static uint8_t DHT11_ReadByte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        // 等待位起始信号（低电平结束）
        uint32_t timeout = 0;
        while (HAL_GPIO_ReadPin(dht11Port, dht11Pin) == GPIO_PIN_RESET) {
            delay_us(1);
            if (++timeout > 60) return 0xFF; // 超时
        }
        // 延时 40us 后判断电平
        delay_us(40);
        if (HAL_GPIO_ReadPin(dht11Port, dht11Pin) == GPIO_PIN_SET) {
            byte |= (1 << (7 - i)); // 高位在前
            // 等待该位高电平结束
            while (HAL_GPIO_ReadPin(dht11Port, dht11Pin) == GPIO_PIN_SET);
        }
    }
    return byte;
}

// 初始化 DHT11
SensorStatusEnum DHT11_Init(GPIO_TypeDef *port, uint16_t pin) {
    dht11Port = port;
    dht11Pin = pin;

    // 初始化为输出高电平
    DHT11_SetOutputMode();
    HAL_GPIO_WritePin(dht11Port, dht11Pin, GPIO_PIN_SET);
    HAL_Delay(100); // 等待传感器稳定

    return SENSOR_OK;
}

// 读取温湿度数据（若失败则返回上次成功值）
SensorStatusEnum DHT11_Read(float *temperature, float *humidity) {
    // if (!dht11Port) return DHT11_NO_RESPONSE;

    SensorStatusEnum status = DHT11_StartSignal();
    if (status != SENSOR_OK) {
        if (temperature) *temperature = lastTemperature;
        if (humidity) *humidity = lastHumidity;
        lastStatus = status;
        errorCount++;
        return status;
    }

    // 读取 5 字节数据
    uint8_t buffer[5];
    for (uint8_t i = 0; i < 5; i++) {
        buffer[i] = DHT11_ReadByte();
        if (buffer[i] == 0xFF) {
            status = SENSOR_TIMEOUT;
            break;
        }
    }

    // 恢复总线为输出高电平
    DHT11_SetOutputMode();
    HAL_GPIO_WritePin(dht11Port, dht11Pin, GPIO_PIN_SET);

    if (status != SENSOR_OK) {
        if (temperature) *temperature = lastTemperature;
        if (humidity) *humidity = lastHumidity;
        lastStatus = status;
        errorCount++;
        return status;
    }

    // 校验和
    if ((buffer[0] + buffer[1] + buffer[2] + buffer[3]) % 256 != buffer[4]) {
        status = SENSOR_CHECKSUM_ERROR;
        if (temperature) *temperature = lastTemperature;
        if (humidity) *humidity = lastHumidity;
        lastStatus = status;
        errorCount++;
        return status;
    }

    // 解析数据（整数部分 + 小数部分）
    float temp = (float) buffer[2] + (float) buffer[3] / 10.0f;
    float hum = (float) buffer[0] + (float) buffer[1] / 10.0f;

    // 更新静态变量
    lastTemperature = temp;
    lastHumidity = hum;
    lastStatus = SENSOR_OK;
    successCount++;

    if (temperature) *temperature = temp;
    if (humidity) *humidity = hum;

    return SENSOR_OK;
}

// 获取最后一次读取的状态
SensorStatusEnum DHT11_GetLastStatus(void) {
    return lastStatus;
}

// 获取读取统计
void DHT11_GetStatistics(uint32_t *outSuccessCount, uint32_t *outErrorCount) {
    if (outSuccessCount) *outSuccessCount = successCount;
    if (outErrorCount) *outErrorCount = errorCount;
}
