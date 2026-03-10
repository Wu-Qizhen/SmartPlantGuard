/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "actuator_types.h"
#include "fan_driver.h"
#include "system_config.h"
#include "tim.h"

// 执行器
typedef enum {
    ACTUATOR_PUMP = 0, // 水泵
    ACTUATOR_FAN, // 风扇
    ACTUATOR_LIGHT, // 补光灯
    ACTUATOR_COUNT // 执行器总数
} ActuatorEnum;

// 初始化执行器系统
bool ActuatorManager_Init(void);

// 控制执行器
bool ActuatorManager_SetState(ActuatorEnum id, ActuatorStateEnum state);

bool ActuatorManager_SetPWM(ActuatorEnum id, uint16_t dutyCycle); // 0-1000

// 获取执行器状态
ActuatorStateEnum ActuatorManager_GetState(ActuatorEnum id);

ActuatorStatus ActuatorManager_GetStatus(ActuatorEnum id);

// 安全控制函数
bool ActuatorManager_SafeToggle(ActuatorEnum id, uint32_t minInterval);

bool ActuatorManager_CheckOverheat(void); // 检查过热保护

// 重置统计信息
void ActuatorManager_ResetStatistics(ActuatorEnum id);

void ActuatorManager_SetPumpLimits(uint32_t minIntervalSec, uint32_t maxDurationSec);

void ActuatorManager_Update(void);

#endif // ACTUATOR_MANAGER_H
