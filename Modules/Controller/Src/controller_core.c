/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "controller_core.h"
#include "system_status.h"
#include "sensor_manager.h"
#include <string.h>

static ControlParams controlParams = {
    .soilMoistureLow = 40.0f,
    .soilMoistureHigh = 60.0f,
    .temperatureHigh = 30.0f,
    .temperatureLow = 20.0f,
    .lightIntensityLow = 500.0f,
    .lightIntensityHigh = 900.0f,
    .minPumpInterval = 5,
    .maxPumpDuration = 20
};

static ControlDecision lastDecision = {
    .needWatering = false,
    .needCooling = false,
    .needLighting = false,
    .decisionReason = "Initial state"
};

// 自动控制逻辑
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

    // 土壤湿度控制
    if (localCopy.soilMoisture.value < controlParams.soilMoistureLow) {
        ActuatorManager_SetState(ACTUATOR_PUMP, ACTUATOR_ON);
        lastDecision.needWatering = true;
        strcpy(lastDecision.decisionReason, "Soil moisture below threshold");
    } else if (localCopy.soilMoisture.value > controlParams.soilMoistureHigh) {
        ActuatorManager_SetState(ACTUATOR_PUMP, ACTUATOR_OFF);
        lastDecision.needWatering = false;
        strcpy(lastDecision.decisionReason, "Soil moisture above threshold");
    }

    // 温度控制
    if (localCopy.temperature.value > controlParams.temperatureHigh) {
        ActuatorManager_SetState(ACTUATOR_FAN, ACTUATOR_ON);
        lastDecision.needCooling = true;
        strcpy(lastDecision.decisionReason, "Temperature above threshold");
    } else if (localCopy.temperature.value < controlParams.temperatureLow) {
        ActuatorManager_SetState(ACTUATOR_FAN, ACTUATOR_OFF);
        lastDecision.needCooling = false;
        strcpy(lastDecision.decisionReason, "Temperature below threshold");
    }

    // 光照控制
    if (localCopy.lightIntensity.value < controlParams.lightIntensityLow) {
        ActuatorManager_SetState(ACTUATOR_LIGHT, ACTUATOR_ON);
        lastDecision.needLighting = true;
        strcpy(lastDecision.decisionReason, "Light intensity below threshold");
    } else if (localCopy.lightIntensity.value > controlParams.lightIntensityHigh) {
        ActuatorManager_SetState(ACTUATOR_LIGHT, ACTUATOR_OFF);
        lastDecision.needLighting = false;
        strcpy(lastDecision.decisionReason, "Light intensity above threshold");
    }
}

// 主控制循环
void ControllerCore_RunCycle(void) {
    switch (gSystemStatus.controlMode) {
        case MODE_AUTO:
            // 自动控制模式
            autoControl();
            break;
        case MODE_CALIBRATION:
            // 校准模式
            // TODO
            break;
        default:
            break;
    }
}

// 获取控制参数
ControlParams ControllerCore_GetParams(void) {
    return controlParams;
}

// 设置控制参数
bool ControllerCore_SetParams(ControlParams *newParams) {
    if (!newParams) {
        return false;
    }

    controlParams = *newParams;
    return true;
}

// 重置为默认参数
void ControllerCore_ResetParamsToDefaults(void) {
    controlParams.soilMoistureLow = 40.0f;
    controlParams.soilMoistureHigh = 60.0f;
    controlParams.temperatureHigh = 30.0f;
    controlParams.temperatureLow = 20.0f;
    controlParams.lightIntensityLow = 500.0f;
    controlParams.lightIntensityHigh = 900.0f;
    controlParams.minPumpInterval = 5;
    controlParams.maxPumpDuration = 20;
}

// 获取控制决策信息
ControlDecision ControllerCore_GetLastDecision(void) {
    return lastDecision;
}
