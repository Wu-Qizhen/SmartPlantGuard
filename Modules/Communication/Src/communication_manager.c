/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "communication_manager.h"
#include "bluetooth_hc05.h"
#include "wifi_esp8266.h"

static CommunicationStateEnum commStates[3] = {
    COMM_STATE_DISCONNECTED,
    COMM_STATE_DISCONNECTED,
    COMM_STATE_DISCONNECTED
};

// 初始化通信系统
bool CommunicationManager_Init(void) {
    // 初始化蓝牙
    BluetoothConfig btConfig = {
        .deviceName = "SmartPlant",
        .pinCode = "1234",
        .baudRate = 9600
    };
    // 这里需要传入实际的 UART 句柄
    // Bluetooth_Init(&huart1, &btConfig);

    // 初始化 WiFi（预留）
    // WiFiConfig wifiConfig = {
    //     .ssid = "",
    //     .password = "",
    //     .serverIp = "192.168.1.1",
    //     .serverPort = 8080,
    //     .baudRate = 115200
    // };
    // WiFi_Init(&huart2, &wifiConfig);

    return true;
}

// 发送数据
bool CommunicationManager_SendData(CommunicationTypeEnum type, uint8_t *data, uint16_t length) {
    if (!data) {
        return false;
    }

    switch (type) {
        case COMM_BLUETOOTH:
            return Bluetooth_SendData(data, length);
        case COMM_WIFI:
            return WiFi_SendData(data, length);
        case COMM_SERIAL:
            // 串口发送
            return false;
        default:
            return false;
    }
}

// 发送命令包
bool CommunicationManager_SendPacket(CommunicationTypeEnum type, CommandPacket *packet) {
    if (!packet) {
        return false;
    }

    switch (type) {
        case COMM_BLUETOOTH:
            return Bluetooth_SendPacket(packet);
        case COMM_WIFI:
            return WiFi_SendPacket(packet);
        case COMM_SERIAL:
            // 串口发送
            return false;
        default:
            return false;
    }
}

// 接收数据
bool CommunicationManager_ReceiveData(CommunicationTypeEnum type, uint8_t *buffer, uint16_t *length) {
    if (!buffer || !length) {
        return false;
    }

    // 这里需要根据实际的通信模块实现接收逻辑
    return false;
}

// 处理接收到的数据
void CommunicationManager_ProcessData(void) {
    // 处理蓝牙数据
    Bluetooth_ProcessReceivedData();

    // 处理 WiFi 数据
    WiFi_ProcessReceivedData();
}

// 获取通信状态
CommunicationStateEnum CommunicationManager_GetState(CommunicationTypeEnum type) {
    if (type >= 3) {
        return COMM_STATE_ERROR;
    }

    return commStates[type];
}

// 连接
bool CommunicationManager_Connect(CommunicationTypeEnum type) {
    switch (type) {
        case COMM_BLUETOOTH:
            // 蓝牙连接
            commStates[type] = COMM_STATE_CONNECTED;
            return true;
        case COMM_WIFI:
            // WiFi 连接
            return WiFi_Connect();
        case COMM_SERIAL:
            // 串口连接
            commStates[type] = COMM_STATE_CONNECTED;
            return true;
        default:
            return false;
    }
}

// 断开连接
bool CommunicationManager_Disconnect(CommunicationTypeEnum type) {
    switch (type) {
        case COMM_BLUETOOTH:
            // 蓝牙断开
            commStates[type] = COMM_STATE_DISCONNECTED;
            return true;
        case COMM_WIFI:
            // WiFi 断开
            return WiFi_Disconnect();
        case COMM_SERIAL:
            // 串口断开
            commStates[type] = COMM_STATE_DISCONNECTED;
            return true;
        default:
            return false;
    }
}
