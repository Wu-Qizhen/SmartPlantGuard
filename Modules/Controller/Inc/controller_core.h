/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef CONTROLLER_CORE_H
#define CONTROLLER_CORE_H

#include "controller_types.h"
#include "system_state.h"
#include "actuator_manager.h"

// 控制器初始化
bool ControllerCore_Init(void);

// 主控制循环（在 FreeRTOS 任务或主循环中调用）
void ControllerCore_RunCycle(void);

// 设置控制模式
bool ControllerCore_SetMode(ControlModeEnum mode);

// 手动控制接口
bool ControllerCore_ManualControl(ActuatorIDEnum actuator, ActuatorStateEnum state);

bool ControllerCore_ManualPWM(ActuatorIDEnum actuator, uint16_t dutyCycle);

// 获取控制参数
ControlParams ControllerCore_GetParams(void);

bool ControllerCore_SetParams(ControlParams *newParams);

// 重置为默认参数
void ControllerCore_ResetToDefaults(void);

// 获取控制决策信息
ControlDecision ControllerCore_GetLastDecision(void);

#endif // CONTROLLER_CORE_H
