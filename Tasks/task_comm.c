/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "bluetooth_bt24.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "main.h"
#include "usart.h"

// 初始化设备名称、配对密码、波特率
static BluetoothConfig btConfig = {
    .deviceName = "PlantGuard",
    .pinCode = "McEnvCtr",
    .baudRate = 9600
};

uint8_t commRxByte; // 全局变量，供中断回调使用

void StartTask_Comm(void *argument) {
    Bluetooth_Init(&huart1, &btConfig);
    HAL_UART_Receive_IT(&huart1, &commRxByte, 1); // 初始化完成后开启中断

    uint8_t rxByte;

    for (;;) {
        // 从队列获取一个字节（阻塞等待）
        if (osMessageQueueGet(Queue_BluetoothRxHandle, &rxByte, NULL, portMAX_DELAY) == osOK) {
            // TODO: 测试数字密码
            /*// 将收到的字节原封不动发回（阻塞发送，简单可靠）
            if (!Bluetooth_SendData(&rxByte, 1)) {
                // 发送失败处理（可重试或记录错误）
            }*/

            // 将字节存入蓝牙接收缓冲区
            Bluetooth_ReceiveByte(rxByte);
            // 尝试解析并响应（如果收到完整命令包，会自动发送响应）
            Bluetooth_ProcessReceivedData();
        }
    }
}
