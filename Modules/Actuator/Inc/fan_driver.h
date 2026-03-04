/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.03.02
 */
#ifndef FAN_DRIVER_H
#define FAN_DRIVER_H

#include "pwm_driver.h"
#include <stdbool.h>

typedef struct {
    PWMConfig pwm; // 复用 PWM 配置
    GPIO_TypeDef *dir1Port;
    uint16_t dir1Pin;
    GPIO_TypeDef *dir2Port;
    uint16_t dir2Pin;
} FanConfig;

bool Fan_Init(FanConfig *config);

bool Fan_SetSpeed(FanConfig *config, uint16_t dutyCycle);

bool Fan_On(FanConfig *config);

bool Fan_Off(FanConfig *config);

#endif // FAN_DRIVER_H
