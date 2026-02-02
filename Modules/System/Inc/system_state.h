#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdbool.h>

// 系统运行状态
typedef enum {
    SYS_STATE_INIT = 0,     // 初始化状态
    SYS_STATE_NORMAL,       // 正常运行
    SYS_STATE_AUTO,         // 自动控制模式
    SYS_STATE_MANUAL,       // 手动控制模式
    SYS_STATE_ERROR,        // 错误状态
    SYS_STATE_CALIBRATING   // 校准状态
} SystemStateEnum;

// 错误码定义
typedef enum {
    ERROR_NONE = 0,
    ERROR_SENSOR_FAILURE,      // 传感器故障
    ERROR_ACTUATOR_FAILURE,    // 执行器故障
    ERROR_COMMUNICATION_FAIL,  // 通信故障
    ERROR_POWER_LOW,           // 电源电压低
    ERROR_WATER_LOW            // 水箱缺水
} ErrorCodeEnum;

// 控制模式
typedef enum {
    MODE_AUTO = 0,     // 自动模式
    MODE_MANUAL,       // 手动模式
    MODE_CALIBRATION   // 校准模式
} ControlModeEnum;

// 系统状态结构体
typedef struct {
    SystemStateEnum currentState;
    ErrorCodeEnum lastError;
    ControlModeEnum controlMode;
    bool isSystemRunning;
    uint32_t uptimeSeconds;    // 系统运行时间
    uint8_t retryCount;        // 重试计数器
} SystemState;

// 全局状态变量
extern SystemState gSystemState;

// 函数声明
void SystemState_Init(void);
void SystemState_Update(SystemStateEnum newState);
void SystemState_SetError(ErrorCodeEnum error);
void SystemState_ClearError(void);
const char* SystemState_GetStateString(SystemStateEnum state);
const char* SystemState_GetErrorString(ErrorCodeEnum error);

#endif // SYSTEM_STATE_H