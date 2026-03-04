/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef WIFI_ESP8266_H
#define WIFI_ESP8266_H

#include "protocol.h"
#include "stm32f1xx_hal.h"

// WiFi 模块状态
typedef enum {
    WIFI_STATE_DISCONNECTED = 0,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_ERROR
} WiFiStateEnum;

// WiFi 配置
typedef struct {
    char ssid[32]; // WiFi 名称
    char password[64]; // WiFi 密码
    char serverIp[16]; // 服务器 IP
    uint16_t serverPort; // 服务器端口
    uint32_t baudRate; // 波特率
} WiFiConfig;

// WiFi 状态
typedef struct {
    WiFiStateEnum state;
    bool isConnected;
    uint32_t bytesReceived;
    uint32_t bytesSent;
    uint32_t connectCount;
} WiFiStatus;

// 初始化 WiFi 模块
bool WiFi_Init(UART_HandleTypeDef *huart, WiFiConfig *config);

// 连接 WiFi
bool WiFi_Connect(void);

// 断开 WiFi
bool WiFi_Disconnect(void);

// 发送数据
bool WiFi_SendData(uint8_t *data, uint16_t length);

bool WiFi_SendPacket(CommandPacket *packet);

// 接收处理（在串口中断中调用）
void WiFi_ReceiveByte(uint8_t byte);

bool WiFi_ProcessReceivedData(void);

// 获取状态
WiFiStatus WiFi_GetStatus(void);

// 配置 WiFi 模块
bool WiFi_SetSSID(const char *ssid);

bool WiFi_SetPassword(const char *password);

bool WiFi_SetServer(const char *ip, uint16_t port);

#endif // WIFI_ESP8266_H
