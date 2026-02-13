/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

// 指令类型
typedef enum {
    CMD_GET_SENSOR_DATA = 0x01, // 获取传感器数据
    CMD_GET_ACTUATOR_STATE = 0x02, // 获取执行器状态
    CMD_SET_ACTUATOR = 0x03, // 设置执行器
    CMD_SET_PARAMS = 0x04, // 设置参数
    CMD_GET_PARAMS = 0x05, // 获取参数
    CMD_RESET = 0x06, // 复位
    CMD_CALIBRATE = 0x07, // 校准
    CMD_GET_SYSTEM_INFO = 0x08, // 获取系统信息
    CMD_ACK = 0x09, // 确认
    CMD_ERROR = 0x0A // 错误
} CommandTypeEnum;

// 指令结构
#pragma pack(push, 1)
typedef struct {
    uint8_t startByte; // 起始字节 0xAA
    uint8_t command; // 指令类型
    uint8_t dataLength; // 数据长度
    uint8_t data[32]; // 数据
    uint8_t checksum; // 校验和
    uint8_t endByte; // 结束字节 0x55
} CommandPacket;
#pragma pack(pop)

// 响应结构
typedef struct {
    CommandTypeEnum command;
    bool success;
    uint8_t data[32];
    uint8_t dataLength;
} Response;

// 协议处理函数
bool Protocol_ParsePacket(uint8_t *buffer, uint16_t length, CommandPacket *packet);

bool Protocol_ValidatePacket(CommandPacket *packet);

uint8_t Protocol_CalculateChecksum(uint8_t *data, uint8_t length);

Response Protocol_ProcessCommand(CommandPacket *packet);

#endif // PROTOCOL_H
