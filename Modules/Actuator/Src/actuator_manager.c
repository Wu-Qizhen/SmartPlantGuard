#include "actuator_manager.h"

static ActuatorStatus actuatorStatuses[ACTUATOR_ID_COUNT];
static bool isInitialized = false;

// 初始化执行器系统
bool ActuatorManager_Init(void) {
    // 初始化各个执行器
    for (int i = 0; i < ACTUATOR_ID_COUNT; i++) {
        actuatorStatuses[i].currentState = ACTUATOR_OFF;
        actuatorStatuses[i].totalOnTime = 0;
        actuatorStatuses[i].operationCount = 0;
        actuatorStatuses[i].isFaulty = false;
    }
    
    isInitialized = true;
    return true;
}

// 控制执行器
bool ActuatorManager_SetState(ActuatorIDEnum id, ActuatorStateEnum state) {
    if (!isInitialized || id >= ACTUATOR_ID_COUNT) {
        return false;
    }
    
    // 检查状态是否变化
    if (actuatorStatuses[id].currentState != state) {
        actuatorStatuses[id].currentState = state;
        actuatorStatuses[id].operationCount++;
        
        // 根据执行器类型执行相应的控制
        switch (id) {
            case ACTUATOR_ID_PUMP:
                // 控制水泵继电器
                break;
            case ACTUATOR_ID_FAN:
                // 控制风扇继电器
                break;
            case ACTUATOR_ID_LIGHT:
                // 控制补光灯继电器
                break;
            default:
                break;
        }
    }
    
    return true;
}

// 设置 PWM 占空比
bool ActuatorManager_SetPWM(ActuatorIDEnum id, uint16_t dutyCycle) {
    if (!isInitialized || id >= ACTUATOR_ID_COUNT) {
        return false;
    }
    
    // 限制占空比范围
    if (dutyCycle > 1000) {
        dutyCycle = 1000;
    }
    
    // 只对风扇执行 PWM 控制
    if (id == ACTUATOR_ID_FAN) {
        // 设置风扇 PWM
    }
    
    return true;
}

// 获取执行器状态
ActuatorStateEnum ActuatorManager_GetState(ActuatorIDEnum id) {
    if (!isInitialized || id >= ACTUATOR_ID_COUNT) {
        return ACTUATOR_ERROR;
    }
    
    return actuatorStatuses[id].currentState;
}

// 获取执行器状态信息
ActuatorStatus ActuatorManager_GetStatus(ActuatorIDEnum id) {
    if (!isInitialized || id >= ACTUATOR_ID_COUNT) {
        ActuatorStatus errorStatus = {
            .currentState = ACTUATOR_ERROR,
            .totalOnTime = 0,
            .operationCount = 0,
            .isFaulty = true
        };
        return errorStatus;
    }
    
    return actuatorStatuses[id];
}

// 安全切换执行器状态
bool ActuatorManager_SafeToggle(ActuatorIDEnum id, uint32_t minInterval) {
    if (!isInitialized || id >= ACTUATOR_ID_COUNT) {
        return false;
    }
    
    // 检查最小间隔
    // 这里需要实现时间检查逻辑
    
    // 切换状态
    ActuatorStateEnum newState = (actuatorStatuses[id].currentState == ACTUATOR_ON) ? ACTUATOR_OFF : ACTUATOR_ON;
    return ActuatorManager_SetState(id, newState);
}

// 检查过热保护
bool ActuatorManager_CheckOverheat(void) {
    // 这里需要实现过热检测逻辑
    return true;
}

// 重置统计信息
void ActuatorManager_ResetStatistics(ActuatorIDEnum id) {
    if (!isInitialized || id >= ACTUATOR_ID_COUNT) {
        return;
    }
    
    actuatorStatuses[id].totalOnTime = 0;
    actuatorStatuses[id].operationCount = 0;
    actuatorStatuses[id].isFaulty = false;
}