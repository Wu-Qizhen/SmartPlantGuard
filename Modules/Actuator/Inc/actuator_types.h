#ifndef ACTUATOR_TYPES_H
#define ACTUATOR_TYPES_H

// 执行器类型
typedef enum {
    ACTUATOR_RELAY = 0,   // 继电器
    ACTUATOR_PWM,         // PWM 设备
    ACTUATOR_SERVO        // 舵机（预留）
} ActuatorTypeEnum;

// 执行器状态
typedef enum {
    ACTUATOR_OFF = 0,
    ACTUATOR_ON,
    ACTUATOR_ERROR
} ActuatorStateEnum;

// 执行器配置
typedef struct {
    ActuatorTypeEnum type;
    GPIO_TypeDef* port;
    uint16_t pin;
    uint8_t pwmChannel;      // 如果是 PWM 设备
    uint16_t minDutyCycle;   // 最小占空比
    uint16_t maxDutyCycle;   // 最大占空比
} ActuatorConfig;

// 执行器状态信息
typedef struct {
    ActuatorConfig config;
    ActuatorStateEnum currentState;
    uint32_t totalOnTime;    // 总开启时间（秒）
    uint32_t operationCount; // 操作次数
    bool isFaulty;           // 是否故障
} ActuatorStatus;

#endif // ACTUATOR_TYPES_H