#include "wifi_esp8266.h"

static UART_HandleTypeDef* wifiUart;
static WiFiStatus wifiStatus = {
    .state = WIFI_STATE_DISCONNECTED,
    .isConnected = false,
    .bytesReceived = 0,
    .bytesSent = 0,
    .connectCount = 0
};

static WiFiConfig wifiConfig;
static uint8_t rxBuffer[128];
static uint16_t rxBufferIndex = 0;

// 初始化 WiFi 模块
bool WiFi_Init(UART_HandleTypeDef* huart, WiFiConfig* config) {
    if (!huart || !config) {
        return false;
    }
    
    wifiUart = huart;
    memcpy(&wifiConfig, config, sizeof(WiFiConfig));
    wifiStatus.state = WIFI_STATE_CONNECTING;
    
    // 配置 WiFi 模块
    // 这里需要根据 ESP8266 的 AT 指令集进行配置
    
    wifiStatus.state = WIFI_STATE_DISCONNECTED;
    
    return true;
}

// 连接 WiFi
bool WiFi_Connect(void) {
    if (!wifiUart) {
        return false;
    }
    
    wifiStatus.state = WIFI_STATE_CONNECTING;
    
    // 发送连接指令
    // 这里需要根据 ESP8266 的 AT 指令集进行连接
    
    // 模拟连接成功
    wifiStatus.state = WIFI_STATE_CONNECTED;
    wifiStatus.isConnected = true;
    wifiStatus.connectCount++;
    
    return true;
}

// 断开 WiFi
bool WiFi_Disconnect(void) {
    if (!wifiUart) {
        return false;
    }
    
    // 发送断开指令
    // 这里需要根据 ESP8266 的 AT 指令集进行断开
    
    wifiStatus.state = WIFI_STATE_DISCONNECTED;
    wifiStatus.isConnected = false;
    
    return true;
}

// 发送数据
bool WiFi_SendData(uint8_t* data, uint16_t length) {
    if (!data || !wifiUart) {
        return false;
    }
    
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        wifiUart, 
        data, 
        length, 
        1000
    );
    
    if (status == HAL_OK) {
        wifiStatus.bytesSent += length;
        return true;
    }
    
    return false;
}

// 发送命令包
bool WiFi_SendPacket(CommandPacket* packet) {
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
    return WiFi_SendData(buffer, length);
}

// 接收处理（在串口中断中调用）
void WiFi_ReceiveByte(uint8_t byte) {
    if (rxBufferIndex < sizeof(rxBuffer)) {
        rxBuffer[rxBufferIndex++] = byte;
    }
}

// 处理接收到的数据
bool WiFi_ProcessReceivedData(void) {
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
        
        WiFi_SendPacket(&ackPacket);
        
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
WiFiStatus WiFi_GetStatus(void) {
    return wifiStatus;
}

// 配置 WiFi 名称
bool WiFi_SetSSID(const char* ssid) {
    if (!ssid) {
        return false;
    }
    
    strncpy(wifiConfig.ssid, ssid, sizeof(wifiConfig.ssid) - 1);
    wifiConfig.ssid[sizeof(wifiConfig.ssid) - 1] = '\0';
    
    return true;
}

// 配置 WiFi 密码
bool WiFi_SetPassword(const char* password) {
    if (!password) {
        return false;
    }
    
    strncpy(wifiConfig.password, password, sizeof(wifiConfig.password) - 1);
    wifiConfig.password[sizeof(wifiConfig.password) - 1] = '\0';
    
    return true;
}

// 配置服务器
bool WiFi_SetServer(const char* ip, uint16_t port) {
    if (!ip) {
        return false;
    }
    
    strncpy(wifiConfig.serverIp, ip, sizeof(wifiConfig.serverIp) - 1);
    wifiConfig.serverIp[sizeof(wifiConfig.serverIp) - 1] = '\0';
    wifiConfig.serverPort = port;
    
    return true;
}