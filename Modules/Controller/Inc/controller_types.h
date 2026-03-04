/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef CONTROLLER_TYPES_H
#define CONTROLLER_TYPES_H

#include <stdbool.h>
#include <stdint.h>

// 控制参数结构
typedef struct {
    // 土壤湿度控制
    float soilMoistureLow; // 低阈值（开启水泵）
    float soilMoistureHigh; // 高阈值（关闭水泵）

    // 温度控制
    float temperatureHigh; // 高阈值（开启风扇）
    float temperatureLow; // 低阈值（关闭风扇）

    // 光照控制（预留）
    float lightIntensityLow; // 低阈值（开启补光）
    float lightIntensityHigh; // 高阈值（关闭补光）

    // 滞回控制参数
    float hysteresisBand; // 滞回带宽

    // 时间控制
    uint32_t minPumpInterval; // 水泵最小间隔（秒）
    uint32_t maxPumpDuration; // 水泵最大持续时间（秒）
} ControlParams;

// 控制决策结果
typedef struct {
    bool needWatering; // 需要浇水
    bool needCooling; // 需要降温
    bool needLighting; // 需要补光
    char decisionReason[64]; // 决策原因描述
} ControlDecision;

#endif // CONTROLLER_TYPES_H
