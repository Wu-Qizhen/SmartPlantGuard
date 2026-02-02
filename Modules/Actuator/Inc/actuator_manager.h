#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include "actuator_types.h"

// 执行器ID
typedef enum {
    ACTUATOR_ID_PUMP = 0,    // 水泵
    ACTUATOR_ID_FAN,         // 风扇
    ACTUATOR_ID_LIGHT,       // 补光灯（预留）
    ACTUATOR_ID_COUNT        // 执行器总数
} ActuatorIDEnum;

// 初始化执行器系统
bool ActuatorManager_Init(void);

// 控制执行器
bool ActuatorManager_SetState(ActuatorIDEnum id, ActuatorStateEnum state);
bool ActuatorManager_SetPWM(ActuatorIDEnum id, uint16_t dutyCycle);  // 0-1000

// 获取执行器状态
ActuatorStateEnum ActuatorManager_GetState(ActuatorIDEnum id);
ActuatorStatus ActuatorManager_GetStatus(ActuatorIDEnum id);

// 安全控制函数
bool ActuatorManager_SafeToggle(ActuatorIDEnum id, uint32_t minInterval);
bool ActuatorManager_CheckOverheat(void);  // 检查过热保护

// 重置统计信息
void ActuatorManager_ResetStatistics(ActuatorIDEnum id);

#endif // ACTUATOR_MANAGER_H