/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "actuator_manager.h"

static ActuatorStatus actuatorStatuses[ACTUATOR_COUNT];
static bool isInitialized = false;

// 初始化执行器系统
bool ActuatorManager_Init(void) {
    FanConfig fan = {
        .pwm = {
            .htim = &htim3,
            .channel = FAN_PWM_CHANNEL,
            .period = 999,
            .minDutyCycle = 0,
            .maxDutyCycle = 1000
        },
        .dir1Port = FAN_AIN1_PORT,
        .dir1Pin = FAN_AIN1_PIN,
        .dir2Port = FAN_AIN2_PORT,
        .dir2Pin = FAN_AIN2_PIN
    };
    Fan_Init(&fan);

    // 初始化各个执行器
    for (int i = 0; i < ACTUATOR_COUNT; i++) {
        actuatorStatuses[i].currentState = ACTUATOR_OFF;
        actuatorStatuses[i].totalOnTime = 0;
        actuatorStatuses[i].operationCount = 0;
        actuatorStatuses[i].isFaulty = false;
    }

    isInitialized = true;
    return true;
}

// 控制执行器
bool ActuatorManager_SetState(ActuatorEnum id, ActuatorStateEnum state) {
    if (!isInitialized || id >= ACTUATOR_COUNT) {
        return false;
    }

    // 检查状态是否变化
    if (actuatorStatuses[id].currentState != state) {
        actuatorStatuses[id].currentState = state;
        actuatorStatuses[id].operationCount++;

        // 根据执行器类型执行相应的控制
        switch (id) {
            case ACTUATOR_PUMP:
                // TODO: 控制水泵继电器
                break;
            case ACTUATOR_FAN:
                // 控制风扇继电器
                // 已弃用
                break;
            case ACTUATOR_LIGHT:
                // TODO: 控制补光灯继电器
                break;
            default:
                break;
        }
    }

    return true;
}

// 设置 PWM 占空比
bool ActuatorManager_SetPWM(ActuatorEnum id, uint16_t dutyCycle) {
    if (!isInitialized || id >= ACTUATOR_COUNT) {
        return false;
    }

    // 限制占空比范围
    if (dutyCycle > 1000) {
        dutyCycle = 1000;
    }

    // 只对风扇执行 PWM 控制
    if (id == ACTUATOR_FAN) {
        FanConfig fan = {
            .pwm = {
                .htim = &htim3,
                .channel = FAN_PWM_CHANNEL,
                .period = 999,
                .minDutyCycle = 0,
                .maxDutyCycle = 1000
            },
            .dir1Port = FAN_AIN1_PORT,
            .dir1Pin = FAN_AIN1_PIN,
            .dir2Port = FAN_AIN2_PORT,
            .dir2Pin = FAN_AIN2_PIN
        };
        Fan_SetSpeed(&fan, dutyCycle);
    }

    return true;
}

// 获取执行器状态
ActuatorStateEnum ActuatorManager_GetState(ActuatorEnum id) {
    if (!isInitialized || id >= ACTUATOR_COUNT) {
        return ACTUATOR_ERROR;
    }

    return actuatorStatuses[id].currentState;
}

// 获取执行器状态信息
ActuatorStatus ActuatorManager_GetStatus(ActuatorEnum id) {
    if (!isInitialized || id >= ACTUATOR_COUNT) {
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
bool ActuatorManager_SafeToggle(ActuatorEnum id, uint32_t minInterval) {
    if (!isInitialized || id >= ACTUATOR_COUNT) {
        return false;
    }

    // NEXT: 检查最小间隔
    // 暂时不实现，后续增加时间戳字段实现

    // 切换状态
    ActuatorStateEnum newState = (actuatorStatuses[id].currentState == ACTUATOR_ON) ? ACTUATOR_OFF : ACTUATOR_ON;
    return ActuatorManager_SetState(id, newState);
}

// NEXT: 检查过热保护
bool ActuatorManager_CheckOverheat(void) {
    // 这里需要实现过热检测逻辑
    return true;
}

// 重置统计信息
void ActuatorManager_ResetStatistics(ActuatorEnum id) {
    if (!isInitialized || id >= ACTUATOR_COUNT) {
        return;
    }

    actuatorStatuses[id].totalOnTime = 0;
    actuatorStatuses[id].operationCount = 0;
    actuatorStatuses[id].isFaulty = false;
}
