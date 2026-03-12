/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 */
#include "stm32f1xx_hal.h"
#include "system_status.h"
#include "system_config.h"

// 全局状态变量
SystemStatus gSystemStatus;
osMutexId_t gSystemStatusMutex;

// 初始化系统状态
void SystemStatus_Init(void) {
    gSystemStatus.currentState = SYS_STATE_INIT;
    gSystemStatus.controlMode = MODE_AUTO;
    gSystemStatus.uptimeMills = 0;
    HAL_GPIO_WritePin(SYSTEM_LED_PORT, SYSTEM_LED_PIN, GPIO_PIN_RESET);
}

// 更新系统状态
void SystemStatus_Update(SystemStateEnum newState) {
    osMutexAcquire(gSystemStatusMutex, osWaitForever);
    gSystemStatus.currentState = newState;
    osMutexRelease(gSystemStatusMutex);
}

// 获取状态字符串
const char *SystemStatus_GetString(SystemStateEnum state) {
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

// 获取控制模式
ControlModeEnum SystemStatus_GetControlMode(void) {
    osMutexAcquire(gSystemStatusMutex, osWaitForever);
    ControlModeEnum mode = gSystemStatus.controlMode;
    osMutexRelease(gSystemStatusMutex);
    return mode;
}

// 设置控制模式
bool SystemStatus_SetControlMode(ControlModeEnum mode) {
    osMutexAcquire(gSystemStatusMutex, osWaitForever);
    gSystemStatus.controlMode = mode;
    osMutexRelease(gSystemStatusMutex);
    return true;
}
