/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include <stdio.h>
#include "cmsis_os2.h"
#include "main.h"
#include "sensor_types.h"
#include "usart.h"

void StartTask_Comm(void *argument) {
    AllSensorData receivedData; // 用于存储接收到的数据
    char txBuffer[128]; // 发送缓冲区
    int len = 0; // 用于存储字符串长度

    for (;;) {
        // 阻塞等待队列消息
        if (osMessageQueueGet(Queue_SensorDataHandle, &receivedData, NULL, osWaitForever) == osOK) {
            // 1. 使用 sprintf 格式化字符串到缓冲区
            len = snprintf(txBuffer, sizeof(txBuffer),
                           "Temperature: %.1f C, Humidity: %.1f %%\r\n",
                           receivedData.temperature.value,
                           receivedData.humidity.value);

            // 2. 通过 HAL_UART_Transmit 发送数据
            if (len > 0) {
                HAL_UART_Transmit(&huart2, (uint8_t *) txBuffer, len, HAL_MAX_DELAY);
            }
        }
    }
}
