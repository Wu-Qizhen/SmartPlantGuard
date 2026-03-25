/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include "actuator_types.h"
#include "stm32f1xx_hal.h"

// PWM 配置
typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    uint16_t period;
    uint16_t minDutyCycle;
    uint16_t maxDutyCycle;
} PWMConfig;

/**
 * 初始化 PWM 驱动
 * @param config PWM 配置结构体
 * @return 初始化是否成功
 */
// bool PWMDriver_Init(PWMConfig *config);

/**
 * 设置 PWM 占空比
 * @param config PWM 配置结构体
 * @param dutyCycle 占空比（0-1000）
 * @return 设置是否成功
 */
bool PWMDriver_SetDutyCycle(PWMConfig *config, uint16_t dutyCycle);

/**
 * 启动 PWM
 * @param config PWM 配置结构体
 * @return 启动是否成功
 */
bool PWMDriver_Start(PWMConfig *config);

/**
 * 停止 PWM
 * @param config PWM 配置结构体
 * @return 停止是否成功
 */
bool PWMDriver_Stop(PWMConfig *config);

#endif // PWM_DRIVER_H
