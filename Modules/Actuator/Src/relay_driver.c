/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "relay_driver.h"
#include "actuator_manager.h"
#include "stm32f1xx_hal.h"

static RelayStatus relayStatuses[ACTUATOR_ID_COUNT];

// 初始化继电器
bool RelayDriver_Init(RelayConfig *config) {
    if (!config) {
        return false;
    }

    // 配置 GPIO 为输出模式
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = config->controlPin,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
    HAL_GPIO_Init(config->controlPort, &GPIO_InitStruct);

    // 初始化为关闭状态
    HAL_GPIO_WritePin(config->controlPort, config->controlPin, config->activeHigh ? GPIO_PIN_RESET : GPIO_PIN_SET);

    return true;
}

// 控制继电器
bool RelayDriver_Set(RelayConfig *config, ActuatorStateEnum state) {
    if (!config) {
        return false;
    }

    // 检查安全条件
    if (!RelayDriver_CheckSafety(config)) {
        return false;
    }

    // 设置继电器状态
    GPIO_PinState pinState;
    if (state == ACTUATOR_ON) {
        pinState = config->activeHigh ? GPIO_PIN_SET : GPIO_PIN_RESET;
    } else {
        pinState = config->activeHigh ? GPIO_PIN_RESET : GPIO_PIN_SET;
    }

    HAL_GPIO_WritePin(config->controlPort, config->controlPin, pinState);

    // 更新状态
    for (int i = 0; i < ACTUATOR_ID_COUNT; i++) {
        if (relayStatuses[i].config.controlPort == config->controlPort &&
            relayStatuses[i].config.controlPin == config->controlPin) {
            relayStatuses[i].state = state;
            relayStatuses[i].lastToggleTime = HAL_GetTick();
            break;
        }
    }

    return true;
}

// 获取继电器状态
ActuatorStateEnum RelayDriver_GetState(RelayConfig *config) {
    if (!config) {
        return ACTUATOR_ERROR;
    }

    // 读取 GPIO 状态
    GPIO_PinState pinState = HAL_GPIO_ReadPin(config->controlPort, config->controlPin);
    return (pinState == (config->activeHigh ? GPIO_PIN_SET : GPIO_PIN_RESET)) ? ACTUATOR_ON : ACTUATOR_OFF;
}

// 安全切换继电器状态
bool RelayDriver_SafeToggle(RelayConfig *config) {
    if (!config) {
        return false;
    }

    // 检查安全条件
    if (!RelayDriver_CheckSafety(config)) {
        return false;
    }

    // 获取当前状态
    ActuatorStateEnum currentState = RelayDriver_GetState(config);
    // 切换状态
    ActuatorStateEnum newState = (currentState == ACTUATOR_ON) ? ACTUATOR_OFF : ACTUATOR_ON;

    return RelayDriver_Set(config, newState);
}

// 检查安全条件
bool RelayDriver_CheckSafety(RelayConfig *config) {
    if (!config) {
        return false;
    }

    // 检查最小关闭时间
    for (int i = 0; i < ACTUATOR_ID_COUNT; i++) {
        if (relayStatuses[i].config.controlPort == config->controlPort &&
            relayStatuses[i].config.controlPin == config->controlPin) {
            uint32_t elapsedTime = HAL_GetTick() - relayStatuses[i].lastToggleTime;
            if (relayStatuses[i].state == ACTUATOR_OFF &&
                config->minOffTime > 0 &&
                elapsedTime < (config->minOffTime * 1000)) {
                return false;
            }
            break;
        }
    }

    return true;
}
