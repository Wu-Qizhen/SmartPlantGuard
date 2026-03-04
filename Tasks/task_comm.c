/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "cmsis_os2.h"
#include "main.h"
#include "usart.h"
#include "bluetooth_hc05.h"
#include "protocol.h"
#include "sensor_manager.h"
#include "actuator_manager.h"
#include "controller_core.h"

#define BT_UART_HANDLE      huart2
#define BT_TASK_DELAY_MS    10                // 轮询间隔 (ms)
#define BT_DEVICE_NAME      "PlantCtrl_Pro"
#define BT_PIN_CODE         "1234"
#define BT_BAUD_RATE        9600

// 初始化设备名称、配对密码、波特率
static BluetoothConfig btConfig = {
    .deviceName = BT_DEVICE_NAME,
    .pinCode    = BT_PIN_CODE,
    .baudRate   = BT_BAUD_RATE
};

/**
 * @brief  通信任务入口
 */
void StartTask_Comm(void *argument) {
    bool processResult;

    // 等待硬件和外设完全稳定
    osDelay(500);

    if (!Bluetooth_Init(&BT_UART_HANDLE, &btConfig)) {
        // 致命错误处理：闪烁LED或进入安全模式
        while(1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // 假设LED在PC13
            osDelay(200);
        }
    }

    for (;;) {
        // 核心：询问驱动层是否有完整数据包需要处理
        // 若有，bluetooth_hc05.c -> Protocol_ParsePacket -> Protocol_ProcessCommand 会自动执行
        processResult = Bluetooth_ProcessReceivedData();

        // 可选：根据 processResult 做额外统计或心跳灯翻转
        if (processResult) {
            // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }

        osDelay(BT_TASK_DELAY_MS);
    }
}
