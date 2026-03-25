/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef CONTROLLER_CORE_H
#define CONTROLLER_CORE_H

#include "controller_types.h"
#include "actuator_manager.h"

extern osMutexId_t controlParamsMutex;
extern osMutexId_t lastDecisionMutex;

/**
 * 控制器初始化
 * @return 初始化是否成功
 */
bool ControllerCore_Init(void);

/**
 * 主控制循环
 */
void ControllerCore_RunCycle(void);

/**
 * 获取控制参数
 * @return 控制参数结构体
 */
ControlParams ControllerCore_GetParams(void);

/**
 * 设置控制参数
 * @param newParams 新的控制参数
 * @return 设置是否成功
 */
bool ControllerCore_SetParams(ControlParams *newParams);

/**
 * 重置为默认参数
 */
void ControllerCore_ResetParamsToDefaults(void);

/**
 * 获取控制决策信息
 * @return 控制决策结构体
 */
ControlDecision ControllerCore_GetLastDecision(void);

#endif // CONTROLLER_CORE_H
