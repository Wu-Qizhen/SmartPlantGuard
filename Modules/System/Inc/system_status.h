/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include "cmsis_os2.h"
#include <stdbool.h>
#include <stdint.h>

// 控制模式
typedef enum {
    MODE_AUTO, // 自动模式
    MODE_MANUAL, // 手动模式
    MODE_CALIBRATION // 校准模式
} ControlModeEnum;

// 系统运行状态
typedef enum {
    SYS_STATE_INIT = 0, // 初始化状态
    SYS_STATE_NORMAL, // 正常运行
    SYS_STATE_AUTO, // 自动控制模式
    SYS_STATE_MANUAL, // 手动控制模式
    SYS_STATE_ERROR, // 错误状态
    SYS_STATE_CALIBRATING // 校准状态
} SystemStateEnum;

// 系统状态结构体
typedef struct {
    SystemStateEnum currentState;
    ControlModeEnum controlMode;
    uint32_t uptimeSeconds; // 系统运行时间
} SystemStatus;

// 系统信息响应数据结构
#pragma pack(push, 1)
typedef struct {
    uint8_t versionMajor; // 主版本号
    uint8_t versionMinor; // 次版本号
    uint8_t versionPatch; // 补丁版本号
    uint8_t reserved; // 保留字节（对齐用）
    uint32_t uptimeSeconds; // 系统运行时间（秒）
    uint8_t systemState; // 当前系统状态（对应 SystemStateEnum）
    uint8_t controlMode; // 当前控制模式（对应 ControlModeEnum）
} SystemInfoPacket;
#pragma pack(pop)

// 全局状态变量
extern SystemStatus gSystemStatus;
extern osMutexId_t gSystemStatusMutex;

// 函数声明
void SystemStatus_Init(void);

void SystemStatus_Update(SystemStateEnum newState);

const char *SystemStatus_GetString(SystemStateEnum state);

// 获取控制模式
ControlModeEnum SystemStatus_GetControlMode(void);

// 设置控制模式
bool SystemStatus_SetControlMode(ControlModeEnum mode);

// TODO: 后续完善系统与各模块错误处理

#endif // SYSTEM_STATE_H
