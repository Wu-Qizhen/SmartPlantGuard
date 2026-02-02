#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include "protocol.h"

// 通信模块类型
typedef enum {
    COMM_BLUETOOTH = 0,
    COMM_WIFI,
    COMM_SERIAL
} CommunicationTypeEnum;

// 通信状态
typedef enum {
    COMM_STATE_DISCONNECTED = 0,
    COMM_STATE_CONNECTING,
    COMM_STATE_CONNECTED,
    COMM_STATE_ERROR
} CommunicationStateEnum;

// 初始化通信系统
bool CommunicationManager_Init(void);

// 发送数据
bool CommunicationManager_SendData(CommunicationTypeEnum type, uint8_t* data, uint16_t length);

// 发送命令包
bool CommunicationManager_SendPacket(CommunicationTypeEnum type, CommandPacket* packet);

// 接收数据
bool CommunicationManager_ReceiveData(CommunicationTypeEnum type, uint8_t* buffer, uint16_t* length);

// 处理接收到的数据
void CommunicationManager_ProcessData(void);

// 获取通信状态
CommunicationStateEnum CommunicationManager_GetState(CommunicationTypeEnum type);

// 连接 / 断开连接
bool CommunicationManager_Connect(CommunicationTypeEnum type);
bool CommunicationManager_Disconnect(CommunicationTypeEnum type);

#endif // COMMUNICATION_MANAGER_H