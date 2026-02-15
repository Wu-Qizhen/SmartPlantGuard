/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef RELAY_DRIVER_H
#define RELAY_DRIVER_H

#include "actuator_types.h"

// 继电器配置
typedef struct {
    GPIO_TypeDef *controlPort; // 控制端口
    uint16_t controlPin; // 控制引脚
    bool activeHigh; // 是否高电平有效
    uint32_t maxOnTime; // 最大开启时间（秒），0 表示无限制
    uint32_t minOffTime; // 最小关闭时间（秒）
} RelayConfig;

// 继电器状态
typedef struct {
    RelayConfig config;
    ActuatorStateEnum state;
    uint32_t lastToggleTime; // 最后切换时间
    bool safetyLock; // 安全锁（防止频繁切换）
} RelayStatus;

// 初始化继电器
bool RelayDriver_Init(RelayConfig *config);

// 继电器控制
bool RelayDriver_Set(RelayConfig *config, ActuatorStateEnum state);

ActuatorStateEnum RelayDriver_GetState(RelayConfig *config);

// 安全控制
bool RelayDriver_SafeToggle(RelayConfig *config);

bool RelayDriver_CheckSafety(RelayConfig *config);

#endif // RELAY_DRIVER_H
