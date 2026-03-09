/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "actuator_manager.h"

static ActuatorStatus actuatorStatuses[ACTUATOR_COUNT];
static bool isInitialized = false;
static FanConfig fanConfig;
static uint32_t pumpMinIntervalMs = 300000; // 默认 5 分钟
static uint32_t pumpMaxDurationMs = 20000; // 默认 20 秒

// 初始化执行器系统
bool ActuatorManager_Init(void) {
    fanConfig = (FanConfig){
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
    Fan_Init(&fanConfig);

    // 初始化各个执行器
    for (int i = 0; i < ACTUATOR_COUNT; i++) {
        actuatorStatuses[i].currentState = ACTUATOR_OFF;
        actuatorStatuses[i].totalOnTime = 0;
        actuatorStatuses[i].operationCount = 0;
        actuatorStatuses[i].isFaulty = false;
        actuatorStatuses[i].lastOperationTime = 0;
        actuatorStatuses[i].startTime = 0;
    }

    isInitialized = true;
    return true;
}

// 控制执行器
bool ActuatorManager_SetState(ActuatorEnum id, ActuatorStateEnum state) {
    if (!isInitialized || id >= ACTUATOR_COUNT) {
        return false;
    }

    uint32_t currentTime = HAL_GetTick();

    // 检查状态是否变化
    if (actuatorStatuses[id].currentState != state) {
        // 水泵开启前检查最小间隔
        if (id == ACTUATOR_PUMP && state == ACTUATOR_ON) {
            if (actuatorStatuses[id].lastOperationTime > 0 &&
                (currentTime - actuatorStatuses[id].lastOperationTime) < pumpMinIntervalMs) {
                return false; // 间隔不足，拒绝开启
            }
            actuatorStatuses[id].startTime = currentTime; // 记录本次开启时刻
        }

        // 水泵关闭时累加运行时间
        if (id == ACTUATOR_PUMP && actuatorStatuses[id].currentState == ACTUATOR_ON && state == ACTUATOR_OFF) {
            if (actuatorStatuses[id].startTime > 0) {
                uint32_t runTimeMs = currentTime - actuatorStatuses[id].startTime;
                actuatorStatuses[id].totalOnTime += runTimeMs / 1000; // 转换为秒
                actuatorStatuses[id].startTime = 0;
            }
        }

        // 执行状态切换
        actuatorStatuses[id].currentState = state;
        actuatorStatuses[id].operationCount++;
        actuatorStatuses[id].lastOperationTime = currentTime;

        // 硬件控制
        switch (id) {
            case ACTUATOR_PUMP:
                if (state == ACTUATOR_ON) {
                    HAL_GPIO_WritePin(RELAY_PUMP_PORT, RELAY_PUMP_PIN, GPIO_PIN_SET);
                } else {
                    HAL_GPIO_WritePin(RELAY_PUMP_PORT, RELAY_PUMP_PIN, GPIO_PIN_RESET);
                }
                break;
            case ACTUATOR_FAN:
                if (state == ACTUATOR_ON) Fan_On(&fanConfig);
                else Fan_Off(&fanConfig);
                break;
            default: // 补光灯
                if (state == ACTUATOR_ON) {
                    HAL_GPIO_WritePin(RELAY_LIGHT_PORT, RELAY_LIGHT_PIN, GPIO_PIN_SET);
                } else {
                    HAL_GPIO_WritePin(RELAY_LIGHT_PORT, RELAY_LIGHT_PIN, GPIO_PIN_RESET);
                }
                break;
        }
    } else {
        // 状态未变化，但水泵若处于开启状态，需检查是否超过最大运行时间
        if (id == ACTUATOR_PUMP && state == ACTUATOR_ON) {
            if (actuatorStatuses[id].startTime > 0 &&
                (currentTime - actuatorStatuses[id].startTime) > pumpMaxDurationMs) {
                // 超时，强制关闭
                actuatorStatuses[id].currentState = ACTUATOR_OFF;
                actuatorStatuses[id].operationCount++;
                actuatorStatuses[id].lastOperationTime = currentTime;

                uint32_t runTimeMs = currentTime - actuatorStatuses[id].startTime;
                actuatorStatuses[id].totalOnTime += runTimeMs / 1000;
                actuatorStatuses[id].startTime = 0;

                HAL_GPIO_WritePin(RELAY_PUMP_PORT, RELAY_PUMP_PIN, GPIO_PIN_RESET);
            }
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
        Fan_SetSpeed(&fanConfig, dutyCycle);
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

// 供外部调用，同步控制参数
void ActuatorManager_SetPumpLimits(uint32_t minIntervalSec, uint32_t maxDurationSec) {
    pumpMinIntervalMs = minIntervalSec * 1000;
    pumpMaxDurationMs = maxDurationSec * 1000;
}
