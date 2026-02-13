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

// 初始化 PWM 驱动
bool PWM_Driver_Init(PWMConfig *config);

// 设置 PWM 占空比
bool PWM_Driver_SetDutyCycle(PWMConfig *config, uint16_t dutyCycle); // 0-1000

// 启动 PWM
bool PWM_Driver_Start(PWMConfig *config);

// 停止 PWM
bool PWM_Driver_Stop(PWMConfig *config);

#endif // PWM_DRIVER_H
