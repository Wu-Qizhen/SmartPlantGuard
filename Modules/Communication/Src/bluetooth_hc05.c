/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "bluetooth_hc05.h"
#include <string.h>
#include "stdio.h"

#define MAX_BUFFER_SIZE    64

static UART_HandleTypeDef *bluetoothUart;
static BluetoothStatus btStatus = {
    .state = BT_STATE_DISCONNECTED,
    .isPaired = false,
    .bytesReceived = 0,
    .bytesSent = 0
};
static uint8_t rxBuffer[MAX_BUFFER_SIZE];
static uint16_t rxBufferIndex = 0;

// 初始化蓝牙模块
bool Bluetooth_Init(UART_HandleTypeDef *huart, BluetoothConfig *config) {
    char cmdBuffer[64];

    if (!huart || !config) {
        return false;
    }

    bluetoothUart = huart;

    // 初始化状态机
    btStatus.state = BT_STATE_CONNECTING;
    btStatus.isPaired = false;
    btStatus.bytesReceived = 0;
    btStatus.bytesSent = 0;

    // TODO

    // 更新最终状态
    btStatus.state = BT_STATE_CONNECTED;
    btStatus.isPaired = true;

    return true;
}

// 发送数据
bool Bluetooth_SendData(uint8_t *data, uint16_t length) {
    if (!data || !bluetoothUart) {
        return false;
    }

    // TODO: 改为非阻塞
    const HAL_StatusTypeDef status = HAL_UART_Transmit(
        bluetoothUart,
        data,
        length,
        100 // 9600 波特率：37 字节 ≈ 38.5 ms
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

// 接收处理
// TODO: 中断中调用
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
        ); // TODO

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
    char cmdBuffer[64];
    snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+NAME=%s", name);

    // TODO

    return true;
}

// 配置蓝牙配对码
bool Bluetooth_SetPinCode(const char *pin) {
    if (!pin) {
        return false;
    }

    // 发送 AT 指令设置配对码
    char cmdBuffer[64];
    snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+PSWD=%s", pin);

    // TODO

    return true;
}
