/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "controller_core.h"
#include "system_status.h"
#include "sensor_manager.h"
#include "actuator_manager.h"
#include <string.h>

static ControlParams controlParams = {
    .soilMoistureLow = 40.0f,
    .soilMoistureHigh = 60.0f,
    .temperatureHigh = 30.0f,
    .temperatureLow = 20.0f,
    .lightIntensityLow = 500.0f,
    .lightIntensityHigh = 800.0f,
    .minPumpInterval = 300,
    .maxPumpDuration = 20
};

static ControlDecision lastDecision = {
    .needWatering = false,
    .needCooling = false,
    .needLighting = false,
    .decisionReason = "Initial state"
};

// 自动控制逻辑
// TODO: 风扇挡位选择控制
static void autoControl() {
    // 获取传感器数据
    if (osMutexAcquire(gSensorDataMutex, osWaitForever) != osOK) {
        return;
    }

    AllSensorData localCopy = gLatestSensorData;
    osMutexRelease(gSensorDataMutex);

    // 判断是否有有效数据
    if (localCopy.lastUpdateTime == 0) {
        // 从未成功读取过传感器数据
        return;
    }

    // 土壤湿度滞回控制
    ActuatorStateEnum pumpState = ActuatorManager_GetState(ACTUATOR_PUMP);
    if (pumpState == ACTUATOR_OFF && localCopy.soilMoisture.value < controlParams.soilMoistureLow) {
        if (ActuatorManager_SetState(ACTUATOR_PUMP, ACTUATOR_ON)) {
            lastDecision.needWatering = true;
            strcpy(lastDecision.decisionReason, "Soil moisture below threshold");
        }
    } else if (pumpState == ACTUATOR_ON && localCopy.soilMoisture.value > controlParams.soilMoistureHigh) {
        if (ActuatorManager_SetState(ACTUATOR_PUMP, ACTUATOR_OFF)) {
            lastDecision.needWatering = false;
            strcpy(lastDecision.decisionReason, "Soil moisture above threshold");
        }
    }

    // 温度滞回控制
    ActuatorStateEnum fanState = ActuatorManager_GetState(ACTUATOR_FAN);
    if (fanState == ACTUATOR_OFF && localCopy.temperature.value > controlParams.temperatureHigh) {
        ActuatorManager_SetState(ACTUATOR_FAN, ACTUATOR_ON);
        lastDecision.needCooling = true;
        strcpy(lastDecision.decisionReason, "Temperature above threshold");
    } else if (fanState == ACTUATOR_ON && localCopy.temperature.value < controlParams.temperatureLow) {
        ActuatorManager_SetState(ACTUATOR_FAN, ACTUATOR_OFF);
        lastDecision.needCooling = false;
        strcpy(lastDecision.decisionReason, "Temperature below threshold");
    }

    // 光照滞回控制
    ActuatorStateEnum lightState = ActuatorManager_GetState(ACTUATOR_LIGHT);
    if (lightState == ACTUATOR_OFF && localCopy.lightIntensity.value < controlParams.lightIntensityLow) {
        ActuatorManager_SetState(ACTUATOR_LIGHT, ACTUATOR_ON);
        lastDecision.needLighting = true;
        strcpy(lastDecision.decisionReason, "Light intensity below threshold");
    } else if (lightState == ACTUATOR_ON && localCopy.lightIntensity.value > controlParams.lightIntensityHigh) {
        ActuatorManager_SetState(ACTUATOR_LIGHT, ACTUATOR_OFF);
        lastDecision.needLighting = false;
        strcpy(lastDecision.decisionReason, "Light intensity above threshold");
    }
}

// 初始化
bool ControllerCore_Init(void) {
    ActuatorManager_SetPumpLimits(controlParams.minPumpInterval, controlParams.maxPumpDuration);
    return true;
}

// 主控制循环
void ControllerCore_RunCycle(void) {
    ControlModeEnum mode = SystemStatus_GetControlMode();
    switch (mode) {
        case MODE_AUTO:
            // 自动控制模式
            autoControl();
            break;
        default:
            // 校准操作由通信协议函数处理
            break;
    }
    // 无论何种模式，都执行执行器超时检查
    ActuatorManager_Update();
}

// 获取控制参数
ControlParams ControllerCore_GetParams(void) {
    return controlParams;
}

// 设置控制参数
bool ControllerCore_SetParams(ControlParams *newParams) {
    if (!newParams) return false;
    controlParams = *newParams;
    ActuatorManager_SetPumpLimits(controlParams.minPumpInterval, controlParams.maxPumpDuration);
    return true;
}

// 重置为默认参数
void ControllerCore_ResetParamsToDefaults(void) {
    controlParams.soilMoistureLow = 40.0f;
    controlParams.soilMoistureHigh = 60.0f;
    controlParams.temperatureHigh = 30.0f;
    controlParams.temperatureLow = 20.0f;
    controlParams.lightIntensityLow = 500.0f;
    controlParams.lightIntensityHigh = 800.0f;
    controlParams.minPumpInterval = 300;
    controlParams.maxPumpDuration = 20;
    ActuatorManager_SetPumpLimits(controlParams.minPumpInterval, controlParams.maxPumpDuration);
}

// 获取控制决策信息
ControlDecision ControllerCore_GetLastDecision(void) {
    return lastDecision;
}
