/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 */
#include "system_state.h"

// 全局状态变量
SystemState gSystemState;

// 初始化系统状态
void SystemState_Init(void) {
    gSystemState.currentState = SYS_STATE_INIT;
    gSystemState.lastError = ERROR_NONE;
    gSystemState.controlMode = MODE_AUTO;
    gSystemState.isSystemRunning = false;
    gSystemState.uptimeSeconds = 0;
    gSystemState.retryCount = 0;
}

// 更新系统状态
void SystemState_Update(SystemStateEnum newState) {
    gSystemState.currentState = newState;
}

// 设置错误状态
void SystemState_SetError(ErrorCodeEnum error) {
    gSystemState.lastError = error;
    gSystemState.currentState = SYS_STATE_ERROR;
}

// 清除错误状态
void SystemState_ClearError(void) {
    gSystemState.lastError = ERROR_NONE;
    gSystemState.currentState = SYS_STATE_NORMAL;
}

// 获取状态字符串
const char *SystemState_GetStateString(SystemStateEnum state) {
    switch (state) {
        case SYS_STATE_INIT:
            return "INIT";
        case SYS_STATE_NORMAL:
            return "NORMAL";
        case SYS_STATE_AUTO:
            return "AUTO";
        case SYS_STATE_MANUAL:
            return "MANUAL";
        case SYS_STATE_ERROR:
            return "ERROR";
        case SYS_STATE_CALIBRATING:
            return "CALIBRATING";
        default:
            return "UNKNOWN";
    }
}

// 获取错误字符串
const char *SystemState_GetErrorString(ErrorCodeEnum error) {
    switch (error) {
        case ERROR_NONE:
            return "NONE";
        case ERROR_SENSOR_FAILURE:
            return "SENSOR FAILURE";
        case ERROR_ACTUATOR_FAILURE:
            return "ACTUATOR FAILURE";
        case ERROR_COMMUNICATION_FAIL:
            return "COMMUNICATION FAIL";
        case ERROR_POWER_LOW:
            return "POWER LOW";
        case ERROR_WATER_LOW:
            return "WATER LOW";
        default:
            return "UNKNOWN ERROR";
    }
}
