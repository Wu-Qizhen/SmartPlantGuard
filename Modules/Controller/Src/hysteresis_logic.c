/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "hysteresis_logic.h"
#include "stm32f1xx_hal.h"

// 初始化滞回控制器
void Hysteresis_Init(HysteresisContext *ctx, float lowThresh, float highThresh, float hysteresis) {
    if (!ctx) {
        return;
    }

    ctx->lowThreshold = lowThresh;
    ctx->highThreshold = highThresh;
    ctx->hysteresisBand = hysteresis;
    ctx->lastOutputValue = 0.0f;
    ctx->lastOutputState = false;
    ctx->minStateTime = 0;
    ctx->stateStartTime = 0;
}

// 滞回控制决策
bool Hysteresis_Update(HysteresisContext *ctx, float currentValue) {
    if (!ctx) {
        return false;
    }

    // 计算当前阈值（考虑滞回）
    float threshold;
    if (ctx->lastOutputState) {
        // 当前为开启状态，使用低阈值
        threshold = ctx->lowThreshold + ctx->hysteresisBand;
    } else {
        // 当前为关闭状态，使用高阈值
        threshold = ctx->highThreshold - ctx->hysteresisBand;
    }

    // 确定新状态
    bool newState = false;
    if (ctx->lastOutputState) {
        // 当前为开启状态，只有当值低于低阈值时才关闭
        newState = !(currentValue < ctx->lowThreshold);
    } else {
        // 当前为关闭状态，只有当值高于高阈值时才开启
        newState = (currentValue > ctx->highThreshold);
    }

    // 检查最小状态保持时间
    if (newState != ctx->lastOutputState) {
        if (ctx->minStateTime > 0) {
            uint32_t elapsedTime = HAL_GetTick() - ctx->stateStartTime;
            if (elapsedTime < ctx->minStateTime) {
                // 未达到最小状态保持时间，保持原状态
                return ctx->lastOutputState;
            }
        }
        // 状态改变，更新状态开始时间
        ctx->stateStartTime = HAL_GetTick();
    }

    // 更新状态
    ctx->lastOutputState = newState;
    ctx->lastOutputValue = currentValue;

    return newState;
}

// 强制设置状态
void Hysteresis_ForceState(HysteresisContext *ctx, bool state) {
    if (!ctx) {
        return;
    }

    ctx->lastOutputState = state;
    ctx->stateStartTime = HAL_GetTick();
}

// 获取滞回状态
bool Hysteresis_GetState(HysteresisContext *ctx) {
    if (!ctx) {
        return false;
    }

    return ctx->lastOutputState;
}

// 获取阈值
float Hysteresis_GetThresholds(HysteresisContext *ctx, float *low, float *high) {
    if (!ctx) {
        return 0.0f;
    }

    if (low) {
        *low = ctx->lowThreshold;
    }
    if (high) {
        *high = ctx->highThreshold;
    }

    return ctx->hysteresisBand;
}

// 设置最小状态保持时间
void Hysteresis_SetMinStateTime(HysteresisContext *ctx, uint32_t minTimeMs) {
    if (!ctx) {
        return;
    }

    ctx->minStateTime = minTimeMs;
}
