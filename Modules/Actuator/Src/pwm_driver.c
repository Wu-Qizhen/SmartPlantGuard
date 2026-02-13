/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "pwm_driver.h"

// 初始化 PWM 驱动
bool PWM_Driver_Init(PWMConfig *config) {
    if (!config || !config->htim) {
        return false;
    }

    // 配置定时器
    // 这里需要根据实际情况配置定时器参数

    // 启动定时器
    HAL_TIM_PWM_Start(config->htim, config->channel);

    return true;
}

// 设置 PWM 占空比
bool PWM_Driver_SetDutyCycle(PWMConfig *config, uint16_t dutyCycle) {
    if (!config || !config->htim) {
        return false;
    }

    // 限制占空比范围
    if (dutyCycle < config->minDutyCycle) {
        dutyCycle = config->minDutyCycle;
    } else if (dutyCycle > config->maxDutyCycle) {
        dutyCycle = config->maxDutyCycle;
    }

    // 计算比较值
    uint32_t compareValue = (config->period * dutyCycle) / 1000;

    // 设置比较值
    switch (config->channel) {
        case TIM_CHANNEL_1:
            config->htim->Instance->CCR1 = compareValue;
            break;
        case TIM_CHANNEL_2:
            config->htim->Instance->CCR2 = compareValue;
            break;
        case TIM_CHANNEL_3:
            config->htim->Instance->CCR3 = compareValue;
            break;
        case TIM_CHANNEL_4:
            config->htim->Instance->CCR4 = compareValue;
            break;
        default:
            return false;
    }

    return true;
}

// 启动 PWM
bool PWM_Driver_Start(PWMConfig *config) {
    if (!config || !config->htim) {
        return false;
    }

    return HAL_TIM_PWM_Start(config->htim, config->channel) == HAL_OK;
}

// 停止 PWM
bool PWM_Driver_Stop(PWMConfig *config) {
    if (!config || !config->htim) {
        return false;
    }

    return HAL_TIM_PWM_Stop(config->htim, config->channel) == HAL_OK;
}
