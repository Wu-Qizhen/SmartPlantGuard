#ifndef HYSTERESIS_LOGIC_H
#define HYSTERESIS_LOGIC_H

// 滞回控制上下文
typedef struct {
    float lastOutputValue;     // 上次输出值
    bool lastOutputState;      // 上次输出状态
    float highThreshold;       // 高阈值
    float lowThreshold;        // 低阈值
    float hysteresisBand;      // 滞回带宽
    uint32_t minStateTime;     // 最小状态保持时间（ms）
    uint32_t stateStartTime;   // 状态开始时间
} HysteresisContext;

// 初始化滞回控制器
void Hysteresis_Init(HysteresisContext* ctx, float lowThresh, float highThresh, float hysteresis);

// 滞回控制决策
bool Hysteresis_Update(HysteresisContext* ctx, float currentValue);

// 强制设置状态（绕过滞回）
void Hysteresis_ForceState(HysteresisContext* ctx, bool state);

// 获取滞回状态
bool Hysteresis_GetState(HysteresisContext* ctx);
float Hysteresis_GetThresholds(HysteresisContext* ctx, float* low, float* high);

// 设置最小状态保持时间
void Hysteresis_SetMinStateTime(HysteresisContext* ctx, uint32_t minTimeMs);

#endif // HYSTERESIS_LOGIC_H