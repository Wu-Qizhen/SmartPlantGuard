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
#include "storage_flash.h"
#include <string.h>

static ControlParams controlParams = {
    .soilMoistureLow = DEFAULT_SOIL_MOISTURE_LOW,
    .soilMoistureHigh = DEFAULT_SOIL_MOISTURE_HIGH,
    .temperatureHigh = DEFAULT_TEMP_HIGH,
    .temperatureLow = DEFAULT_TEMP_LOW,
    .lightIntensityLow = DEFAULT_LIGHT_INTENSITY_LOW,
    .lightIntensityHigh = DEFAULT_LIGHT_INTENSITY_HIGH,
    .pumpMinInterval = DEFAULT_PUMP_MIN_INTERVAL,
    .pumpMaxDuration = DEFAULT_PUMP_MAX_DURATION
};
static osMutexId_t controlParamsMutex = NULL;

static ControlDecision lastDecision = {
    .needWatering = false,
    .needCooling = false,
    .needLighting = false,
    .decisionReason = "Initial state"
};
static osMutexId_t lastDecisionMutex = NULL;

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
    if (localCopy.lastUpdateTimeMs == 0) {
        // 从未成功读取过传感器数据
        return;
    }

    osMutexAcquire(controlParamsMutex, osWaitForever);
    ControlParams localParams = controlParams;
    osMutexRelease(controlParamsMutex);

    // 土壤湿度滞回控制
    ActuatorStateEnum pumpState = ActuatorManager_GetState(ACTUATOR_PUMP);
    if (pumpState == ACTUATOR_OFF && localCopy.soilMoisture.value < localParams.soilMoistureLow) {
        if (ActuatorManager_SetState(ACTUATOR_PUMP, ACTUATOR_ON)) {
            osMutexAcquire(lastDecisionMutex, osWaitForever);
            lastDecision.needWatering = true;
            strcpy(lastDecision.decisionReason, "Soil moisture below threshold");
            osMutexRelease(lastDecisionMutex);
        }
    } else if (pumpState == ACTUATOR_ON && localCopy.soilMoisture.value > localParams.soilMoistureHigh) {
        if (ActuatorManager_SetState(ACTUATOR_PUMP, ACTUATOR_OFF)) {
            osMutexAcquire(lastDecisionMutex, osWaitForever);
            lastDecision.needWatering = false;
            strcpy(lastDecision.decisionReason, "Soil moisture above threshold");
            osMutexRelease(lastDecisionMutex);
        }
    }

    // 温度滞回控制
    ActuatorStateEnum fanState = ActuatorManager_GetState(ACTUATOR_FAN);
    if (fanState == ACTUATOR_OFF && localCopy.temperature.value > localParams.temperatureHigh) {
        ActuatorManager_SetState(ACTUATOR_FAN, ACTUATOR_ON);
        osMutexAcquire(lastDecisionMutex, osWaitForever);
        lastDecision.needCooling = true;
        strcpy(lastDecision.decisionReason, "Temperature above threshold");
        osMutexRelease(lastDecisionMutex);
    } else if (fanState == ACTUATOR_ON && localCopy.temperature.value < localParams.temperatureLow) {
        ActuatorManager_SetState(ACTUATOR_FAN, ACTUATOR_OFF);
        osMutexAcquire(lastDecisionMutex, osWaitForever);
        lastDecision.needCooling = false;
        strcpy(lastDecision.decisionReason, "Temperature below threshold");
        osMutexRelease(lastDecisionMutex);
    }

    // 光照滞回控制
    ActuatorStateEnum lightState = ActuatorManager_GetState(ACTUATOR_LIGHT);
    if (lightState == ACTUATOR_OFF && localCopy.lightIntensity.value < localParams.lightIntensityLow) {
        ActuatorManager_SetState(ACTUATOR_LIGHT, ACTUATOR_ON);
        osMutexAcquire(lastDecisionMutex, osWaitForever);
        lastDecision.needLighting = true;
        strcpy(lastDecision.decisionReason, "Light intensity below threshold");
        osMutexRelease(lastDecisionMutex);
    } else if (lightState == ACTUATOR_ON && localCopy.lightIntensity.value > localParams.lightIntensityHigh) {
        ActuatorManager_SetState(ACTUATOR_LIGHT, ACTUATOR_OFF);
        osMutexAcquire(lastDecisionMutex, osWaitForever);
        lastDecision.needLighting = false;
        strcpy(lastDecision.decisionReason, "Light intensity above threshold");
        osMutexRelease(lastDecisionMutex);
    }
}

// 初始化
bool ControllerCore_Init(void) {
    controlParamsMutex = osMutexNew(NULL);
    if (controlParamsMutex == NULL) return false;
    lastDecisionMutex = osMutexNew(NULL);
    if (lastDecisionMutex == NULL) return false;

    // osMutexAcquire(controlParamsMutex, osWaitForever);
    // 在 osKernelStart() 之前，不要使用 osMutexAcquire()，因为内核还没有开始运行
    // if (!StorageFlash_LoadConfig(&controlParams)) {
    // osMutexRelease(controlParamsMutex);
    // return false;
    // }
    ActuatorManager_SetPumpLimits(controlParams.pumpMinInterval, controlParams.pumpMaxDuration);
    // osMutexRelease(controlParamsMutex);
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
    osMutexAcquire(controlParamsMutex, osWaitForever);
    ControlParams params = controlParams; // 结构体拷贝
    osMutexRelease(controlParamsMutex);
    return params;
}

// 设置控制参数
bool ControllerCore_SetParams(ControlParams *newParams) {
    if (!newParams) return false;
    osMutexAcquire(controlParamsMutex, osWaitForever);
    controlParams = *newParams;
    ActuatorManager_SetPumpLimits(controlParams.pumpMinInterval, controlParams.pumpMaxDuration);
    osMutexRelease(controlParamsMutex);
    return true;
}

// 重置为默认参数
void ControllerCore_ResetParamsToDefaults(void) {
    osMutexAcquire(controlParamsMutex, osWaitForever);
    controlParams.soilMoistureLow = DEFAULT_SOIL_MOISTURE_LOW;
    controlParams.soilMoistureHigh = DEFAULT_SOIL_MOISTURE_HIGH;
    controlParams.temperatureHigh = DEFAULT_TEMP_HIGH;
    controlParams.temperatureLow = DEFAULT_TEMP_LOW;
    controlParams.lightIntensityLow = DEFAULT_LIGHT_INTENSITY_LOW;
    controlParams.lightIntensityHigh = DEFAULT_LIGHT_INTENSITY_HIGH;
    controlParams.pumpMinInterval = DEFAULT_PUMP_MIN_INTERVAL;
    controlParams.pumpMaxDuration = DEFAULT_PUMP_MAX_DURATION;
    ActuatorManager_SetPumpLimits(controlParams.pumpMinInterval, controlParams.pumpMaxDuration);
    StorageFlash_SaveConfig(&controlParams);
    osMutexRelease(controlParamsMutex);
}

// 获取控制决策信息
ControlDecision ControllerCore_GetLastDecision(void) {
    osMutexAcquire(lastDecisionMutex, osWaitForever);
    ControlDecision decision = lastDecision;
    osMutexRelease(lastDecisionMutex);
    return decision;
}
