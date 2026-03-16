/**
 * 通信协议说明
 * 1. 命令包格式（发送端 -> 设备）：
 *    起始（0xAA）+ 命令（1 字节）+ 数据长度（1 字节）+ 数据（0~N 字节）+ 校验和（1 字节）+ 结束（0x55）
 * 2. 响应包格式（设备 -> 发送端）：
 *    成功：命令字节 = CMD_ACK（0x09），数据部分为对应命令的返回数据（格式见后）
 *    失败：命令字节 = CMD_ERROR（0x0A），数据部分可能包含错误码
 * 3. 校验和计算方法：
 *    对“命令字节、数据长度、所有数据字节”依次进行异或，结果即为校验和
 * 4. 发送端可发送的命令：
 *    ----------------------------------------------------------------------------------------------------
 *     命令名称                命令字节  功能说明          请求数据格式         成功响应数据格式
 *    ----------------------------------------------------------------------------------------------------
 *     CMD_GET_SENSOR_DATA    0x01    获取最新传感器数据  无                 CompactSensorData（13 字节）
 *     CMD_GET_ACTUATOR_STATE 0x02    获取所有执行器状态  无                 3 字节状态数组
 *     CMD_SET_ACTUATOR       0x03    设置指定执行器状态  [ID(1)][State(1)] 无（仅成功 / 失败）
 *     CMD_SET_PARAMS         0x04    设置控制参数       ControlParams     无
 *     CMD_GET_PARAMS         0x05    获取当前控制参数    无                 ControlParams
 *     CMD_RESET              0x06    恢复系统参数为默认值 无                无
 *     CMD_CALIBRATE          0x07    执行校准          自定义（至少 1 字节） 无
 *     CMD_GET_SYSTEM_INFO    0x08    获取系统信息       无                 无
 *     CMD_SET_CONTROL_MODE   0x0B    设置控制模式      [Mode(1)]          无
 *    ----------------------------------------------------------------------------------------------------
 * 5. 各命令响应数据：
 *    (1) CMD_GET_SENSOR_DATA 成功响应数据（CompactSensorData，共 13 字节）：
 *        偏移 0-1：土壤湿度（uint16_t）= 实际值 × 10（0~1000）
 *        偏移 2-3：温度（int16_t）= 实际值 × 10（-500~1500）
 *        偏移 4-5：空气湿度（uint16_t）= 实际值 × 10（0~1000）
 *        偏移 6-7：光照强度（uint16_t）= 原始值
 *        偏移 8  ：状态标志（uint8_t）
 *                 bit0 = 1 表示土壤传感器有效
 *                 bit1 = 1 表示温度传感器有效
 *                 bit2 = 1 表示湿度传感器有效
 *                 bit3 = 1 表示光敏传感器有效
 *        偏移 9-12：时间戳（uint32_t）秒
 *    (2) CMD_GET_ACTUATOR_STATE 成功响应数据（3 字节）：
 *        字节 0：水泵状态（0 = OFF，1 = ON，2 = ERROR）
 *        字节 1：风扇状态（同上）
 *        字节 2：补光灯状态（同上）
 *    (3) CMD_SET_ACTUATOR 请求数据格式：
 *        字节 0：执行器（0 = 水泵，1 = 风扇，2 = 补光灯）
 *        字节 1：目标状态（0 = OFF，1 = ON，2 = ERROR）
 *    (4) CMD_SET_PARAMS / CMD_GET_PARAMS
 *    (5) CMD_CALIBRATE
 *    (6) CMD_GET_SYSTEM_INFO
 * 6. 命令包示例：
 *    (1) 获取传感器数据：AA 01 00 01 55
 *        校验和：0x01 ^ 0x00 = 0x01
 *    (2) 获取执行器状态：AA 02 00 02 55
 *        校验和：0x02 ^ 0x00 = 0x02
 *    (3) 设置执行器（以开启水泵为例）：AA 03 02 00 01 00 55
 *        校验和：0x03 ^ 0x02 ^ 0x00 ^ 0x01 = 0x00
 *    (4) 设置控制参数：
 *        假设 ControlParams 结构体为 8 字节，示例数据：10 20 30 40 50 60 70 80
 *        AA 04 08 10 20 30 40 50 60 70 80 8C 55
 *        校验和：0x04 ^ 0x08 ^ 0x10 ^ 0x20 ^ 0x30 ^ 0x40 ^ 0x50 ^ 0x60 ^ 0x70 ^ 0x80 = 0x8C
 *    (5) 获取控制参数：AA 05 00 05 55
 *        校验和：0x05 ^ 0x00 = 0x05
 *    (6) 复位系统：AA 06 00 06 55
 *        校验和：0x06 ^ 0x00 = 0x06
 *    (7) 校准：
 *        假设校准类型为 0，数据长度 1，数据为 00
 *        AA 07 01 00 06 55
 *        校验和：0x07 ^ 0x01 ^ 0x00 = 0x06
 *    (8) 获取系统信息：AA 08 00 08 55
 *        校验和：0x08 ^ 0x00 = 0x08
 *    (9) 设置控制模式为手动：AA 0B 01 01 0B 55
 *        校验和：0x0B ^ 0x01 ^ 0x01 = 0x0B
 *
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_DATA_SIZE 32
#define MIN_PACKET_SIZE 5
#define PACKET_HEAD 0xAA
#define PACKET_TAIL 0x55

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
    CMD_SET_CONTROL_MODE = 0x0B, // 新增命令
    CMD_ACK = 0x09, // 确认
    CMD_ERROR = 0x0A // 错误
} CommandTypeEnum;

// 校准类型
typedef enum {
    CALIBRATE_SOIL_DRY = 0, // 土壤湿度干态校准
    CALIBRATE_SOIL_WET = 1, // 土壤湿度湿态校准
    CALIBRATE_LIGHT_MIN = 2, // 光照强度最小校准
    CALIBRATE_LIGHT_MAX = 3, // 光照强度最大校准
    CALIBRATE_SOIL_SET = 4, // 直接设置土壤校准值
    CALIBRATE_LIGHT_SET = 5 // 直接设置光照校准值
} CalibrationTypeEnum;

// 指令结构
#pragma pack(push, 1)
typedef struct {
    uint8_t startByte; // 起始字节 0xAA
    uint8_t command; // 指令类型
    uint8_t dataLength; // 数据长度
    uint8_t data[MAX_DATA_SIZE]; // 数据
    uint8_t checksum; // 校验和
    uint8_t endByte; // 结束字节 0x55
} CommandPacket;
#pragma pack(pop)

// 响应结构
typedef struct {
    CommandTypeEnum command;
    bool success;
    uint8_t data[MAX_DATA_SIZE];
    uint8_t dataLength;
} Response;

// 协议处理函数
bool Protocol_ParsePacket(uint8_t *buffer, uint16_t length, CommandPacket *packet);

bool Protocol_ValidatePacket(CommandPacket *packet);

uint8_t Protocol_CalculateChecksum(const uint8_t *data, uint8_t length);

Response Protocol_ProcessCommand(CommandPacket *packet);

// 检查并执行保存配置
void Protocol_CheckSaveConfig(void);

#endif // PROTOCOL_H
