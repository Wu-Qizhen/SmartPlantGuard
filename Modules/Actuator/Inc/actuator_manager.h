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

/**
 * 初始化执行器系统
 * @return 初始化是否成功
 */
bool ActuatorManager_Init(void);

/**
 * 控制执行器
 * @param id 执行器 ID
 * @param state 执行器状态
 * @return 控制是否成功
 */
bool ActuatorManager_SetState(ActuatorEnum id, ActuatorStateEnum state);

/**
 * 设置执行器 PWM 占空比
 * @param id 执行器 ID
 * @param dutyCycle 占空比（0-1000）
 * @return 设置是否成功
 */
bool ActuatorManager_SetPWM(ActuatorEnum id, uint16_t dutyCycle);

/**
 * 获取执行器状态
 * @param id 执行器 ID
 * @return 执行器当前状态
 */
ActuatorStateEnum ActuatorManager_GetState(ActuatorEnum id);

/**
 * 获取执行器状态信息
 * @param id 执行器 ID
 * @return 执行器状态信息结构体
 */
ActuatorStatus ActuatorManager_GetStatus(ActuatorEnum id);

/**
 * 安全切换执行器状态
 * @param id 执行器 ID
 * @param minInterval 最小切换间隔
 * @return 切换是否成功
 */
bool ActuatorManager_SafeToggle(ActuatorEnum id, uint32_t minInterval);

/**
 * 检查过热保护
 * @return 过热检查结果
 */
bool ActuatorManager_CheckOverheat(void);

/**
 * 重置执行器统计信息
 * @param id 执行器 ID
 */
void ActuatorManager_ResetStatistics(ActuatorEnum id);

/**
 * 设置水泵运行限制
 * @param minIntervalSec 最小运行间隔（秒）
 * @param maxDurationSec 最大运行时长（秒）
 */
void ActuatorManager_SetPumpLimits(uint32_t minIntervalSec, uint32_t maxDurationSec);

/**
 * 更新执行器状态
 */
void ActuatorManager_Update(void);

#endif // ACTUATOR_MANAGER_H
