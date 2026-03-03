/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "bluetooth_hc05.h"
#include <string.h>

static UART_HandleTypeDef *bluetoothUart;
static BluetoothStatus btStatus = {
    .state = BT_STATE_DISCONNECTED,
    .isPaired = false,
    .bytesReceived = 0,
    .bytesSent = 0,
    .connectCount = 0
};

static uint8_t rxBuffer[64];
static uint16_t rxBufferIndex = 0;

// 内部辅助宏
#define AT_CMD_TIMEOUT      500
#define AT_RESPONSE_SIZE    64

/**
 * @brief 内部辅助函数：发送 AT 命令并等待 "OK" 响应
 * @param cmd: 要发送的命令字符串 (不含 \r\n)
 * @return true: 收到 OK, false: 超时或收到 ERROR
 */
static bool SendAT_WaitOK(const char *cmd) {
    uint8_t response[AT_RESPONSE_SIZE] = {0};
    uint32_t startTime = HAL_GetTick();
    uint8_t rxByte;
    uint8_t idx = 0;
    bool foundOK = false;

    // 1. 发送命令 + 回车换行
    HAL_UART_Transmit(bluetoothUart, (uint8_t*)cmd, strlen(cmd), AT_CMD_TIMEOUT);
    HAL_UART_Transmit(bluetoothUart, (uint8_t*)"\r\n", 2, AT_CMD_TIMEOUT);

    // 2. 循环接收响应，直到超时或收到完整行
    while (HAL_GetTick() - startTime < AT_CMD_TIMEOUT) {
        if (HAL_UART_Receive(bluetoothUart, &rxByte, 1, 10) == HAL_OK) {
            if (idx < AT_RESPONSE_SIZE - 1) {
                response[idx++] = rxByte;

                // 简单判断：如果接收到 "OK" 关键字
                if (strstr((char*)response, "OK") != NULL) {
                    foundOK = true;
                    break;
                }
                // 如果接收到 "ERROR"
                if (strstr((char*)response, "ERROR") != NULL) {
                    break;
                }
            }
        }
    }

    return foundOK;
}

// 初始化蓝牙模块
bool Bluetooth_Init(UART_HandleTypeDef *huart, BluetoothConfig *config) {
    if (!huart || !config) {
        return false;
    }

    bluetoothUart = huart;
    btStatus.state = BT_STATE_CONNECTING;

    // 配置蓝牙模块
    // 这里需要根据 HC-05 的 AT 指令集进行配置

    btStatus.state = BT_STATE_CONNECTED;
    btStatus.connectCount++;

    return true;
}

// 发送数据
bool Bluetooth_SendData(uint8_t *data, uint16_t length) {
    if (!data || !bluetoothUart) {
        return false;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(
        bluetoothUart,
        data,
        length,
        1000
    );

    if (status == HAL_OK) {
        btStatus.bytesSent += length;
        return true;
    }

    return false;
}

// 发送命令包
bool Bluetooth_SendPacket(CommandPacket *packet) {
    if (!packet) {
        return false;
    }

    // 构建数据包
    uint8_t buffer[40];
    uint16_t length = 0;

    buffer[length++] = packet->startByte;
    buffer[length++] = packet->command;
    buffer[length++] = packet->dataLength;

    memcpy(&buffer[length], packet->data, packet->dataLength);
    length += packet->dataLength;

    buffer[length++] = packet->checksum;
    buffer[length++] = packet->endByte;

    // 发送数据包
    return Bluetooth_SendData(buffer, length);
}

// 接收处理（在串口中断中调用）
void Bluetooth_ReceiveByte(uint8_t byte) {
    if (rxBufferIndex < sizeof(rxBuffer)) {
        rxBuffer[rxBufferIndex++] = byte;
    }
}

// 处理接收到的数据
bool Bluetooth_ProcessReceivedData(void) {
    if (rxBufferIndex == 0) {
        return false;
    }

    // 解析数据包
    CommandPacket packet;
    if (Protocol_ParsePacket(rxBuffer, rxBufferIndex, &packet)) {
        // 处理命令
        Response response = Protocol_ProcessCommand(&packet);

        // 发送响应
        CommandPacket ackPacket = {
            .startByte = 0xAA,
            .command = CMD_ACK,
            .dataLength = response.dataLength,
            .endByte = 0x55
        };
        memcpy(ackPacket.data, response.data, response.dataLength);
        ackPacket.checksum = Protocol_CalculateChecksum(
            &ackPacket.command,
            1 + ackPacket.dataLength
        );

        Bluetooth_SendPacket(&ackPacket);

        // 清空接收缓冲区
        rxBufferIndex = 0;
        return true;
    }

    // 检查是否需要清空缓冲区
    if (rxBufferIndex >= sizeof(rxBuffer)) {
        rxBufferIndex = 0;
    }

    return false;
}

// 获取状态
BluetoothStatus Bluetooth_GetStatus(void) {
    return btStatus;
}

// 配置蓝牙设备名称
bool Bluetooth_SetDeviceName(const char *name) {
    if (!name) {
        return false;
    }

    // 发送 AT 指令设置设备名称
    // 这里需要根据 HC-05 的 AT 指令集进行配置

    return true;
}

// 配置蓝牙配对码
bool Bluetooth_SetPinCode(const char *pin) {
    if (!pin) {
        return false;
    }

    // 发送 AT 指令设置配对码
    // 这里需要根据 HC-05 的 AT 指令集进行配置

    return true;
}
