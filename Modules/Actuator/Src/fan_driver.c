/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.03.02
 */
#include "fan_driver.h"

bool Fan_Init(FanConfig *config) {
    if (!config) return false;

    // 设置方向（正转）
    HAL_GPIO_WritePin(config->dir1Port, config->dir1Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(config->dir2Port, config->dir2Pin, GPIO_PIN_RESET);

    // 设置初始占空比为 0（或最小安全值）
    PWMDriver_SetDutyCycle(&config->pwm, 0);

    // 启动 PWM
    return PWMDriver_Start(&config->pwm);
}

bool Fan_SetSpeed(FanConfig *config, uint16_t dutyCycle) {
    return PWMDriver_SetDutyCycle(&config->pwm, dutyCycle);
}

bool Fan_Start(FanConfig *config) {
    return PWMDriver_Start(&config->pwm);
}

bool Fan_Stop(FanConfig *config) {
    // 停止时可将占空比设为 0 并停止 PWM，或保持 PWM 但占空比为0
    PWMDriver_SetDutyCycle(&config->pwm, 0);
    return PWMDriver_Stop(&config->pwm);
}
