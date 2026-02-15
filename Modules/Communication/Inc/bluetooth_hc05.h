/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef BLUETOOTH_HC05_H
#define BLUETOOTH_HC05_H

#include <stdbool.h>
#include <stdint.h>
#include "protocol.h"
#include "stm32f1xx_hal.h"

// 蓝牙模块状态
typedef enum {
    BT_STATE_DISCONNECTED = 0,
    BT_STATE_CONNECTING,
    BT_STATE_CONNECTED,
    BT_STATE_ERROR
} BluetoothStateEnum;

// 蓝牙配置
typedef struct {
    char deviceName[16]; // 设备名称
    char pinCode[8]; // 配对码
    uint32_t baudRate; // 波特率
} BluetoothConfig;

// 蓝牙管理器状态
typedef struct {
    BluetoothStateEnum state;
    bool isPaired;
    uint32_t bytesReceived;
    uint32_t bytesSent;
    uint32_t connectCount;
} BluetoothStatus;

// 初始化蓝牙模块
bool Bluetooth_Init(UART_HandleTypeDef *huart, BluetoothConfig *config);

// 发送数据
bool Bluetooth_SendData(uint8_t *data, uint16_t length);

bool Bluetooth_SendPacket(CommandPacket *packet);

// 接收处理（在串口中断中调用）
void Bluetooth_ReceiveByte(uint8_t byte);

bool Bluetooth_ProcessReceivedData(void);

// 获取状态
BluetoothStatus Bluetooth_GetStatus(void);

// 配置蓝牙模块
bool Bluetooth_SetDeviceName(const char *name);

bool Bluetooth_SetPinCode(const char *pin);

#endif // BLUETOOTH_HC05_H
