#include "controller_core.h"
#include "sensor_manager.h"

static ControlParams controlParams = {
    .soilMoistureLow = 30.0f,
    .soilMoistureHigh = 40.0f,
    .temperatureHigh = 30.0f,
    .temperatureLow = 25.0f,
    .lightIntensityLow = 100.0f,
    .lightIntensityHigh = 500.0f,
    .hysteresisBand = 2.0f,
    .minPumpInterval = 60,
    .maxPumpDuration = 30
};

static ControlDecision lastDecision = {
    .needWatering = false,
    .needCooling = false,
    .needLighting = false,
    .decisionReason = "Initial state"
};

// 控制器初始化
bool ControllerCore_Init(void) {
    // 初始化执行器
    ActuatorManager_Init();
    
    // 初始化传感器
    SensorManager_Init();
    
    return true;
}

// 主控制循环
void ControllerCore_RunCycle(void) {
    // 获取传感器数据
    AllSensorData sensorData;
    if (SensorManager_ReadAll(&sensorData)) {
        // 根据控制模式执行不同的控制逻辑
        switch (gSystemState.controlMode) {
            case MODE_AUTO:
                // 自动控制模式
                runAutoControl(&sensorData);
                break;
            case MODE_MANUAL:
                // 手动控制模式
                break;
            case MODE_CALIBRATION:
                // 校准模式
                break;
            default:
                break;
        }
    }
}

// 自动控制逻辑
static void runAutoControl(AllSensorData* sensorData) {
    // 土壤湿度控制
    if (sensorData->soilMoisture.value < controlParams.soilMoistureLow) {
        ActuatorManager_SetState(ACTUATOR_ID_PUMP, ACTUATOR_ON);
        lastDecision.needWatering = true;
        strcpy(lastDecision.decisionReason, "Soil moisture below threshold");
    } else if (sensorData->soilMoisture.value > controlParams.soilMoistureHigh) {
        ActuatorManager_SetState(ACTUATOR_ID_PUMP, ACTUATOR_OFF);
        lastDecision.needWatering = false;
    }
    
    // 温度控制
    if (sensorData->temperature.value > controlParams.temperatureHigh) {
        ActuatorManager_SetState(ACTUATOR_ID_FAN, ACTUATOR_ON);
        lastDecision.needCooling = true;
        strcpy(lastDecision.decisionReason, "Temperature above threshold");
    } else if (sensorData->temperature.value < controlParams.temperatureLow) {
        ActuatorManager_SetState(ACTUATOR_ID_FAN, ACTUATOR_OFF);
        lastDecision.needCooling = false;
    }
    
    // 光照控制（预留）
    if (sensorData->lightIntensity.value < controlParams.lightIntensityLow) {
        lastDecision.needLighting = true;
    } else if (sensorData->lightIntensity.value > controlParams.lightIntensityHigh) {
        lastDecision.needLighting = false;
    }
}

// 设置控制模式
bool ControllerCore_SetMode(ControlModeEnum mode) {
    gSystemState.controlMode = mode;
    return true;
}

// 手动控制执行器
bool ControllerCore_ManualControl(ActuatorIDEnum actuator, ActuatorStateEnum state) {
    return ActuatorManager_SetState(actuator, state);
}

// 手动设置 PWM
bool ControllerCore_ManualPWM(ActuatorIDEnum actuator, uint16_t dutyCycle) {
    return ActuatorManager_SetPWM(actuator, dutyCycle);
}

// 获取控制参数
ControlParams ControllerCore_GetParams(void) {
    return controlParams;
}

// 设置控制参数
bool ControllerCore_SetParams(ControlParams* newParams) {
    if (!newParams) {
        return false;
    }
    
    controlParams = *newParams;
    return true;
}

// 重置为默认参数
void ControllerCore_ResetToDefaults(void) {
    controlParams.soilMoistureLow = 30.0f;
    controlParams.soilMoistureHigh = 40.0f;
    controlParams.temperatureHigh = 30.0f;
    controlParams.temperatureLow = 25.0f;
    controlParams.lightIntensityLow = 100.0f;
    controlParams.lightIntensityHigh = 500.0f;
    controlParams.hysteresisBand = 2.0f;
    controlParams.minPumpInterval = 60;
    controlParams.maxPumpDuration = 30;
}

// 获取控制决策信息
ControlDecision ControllerCore_GetLastDecision(void) {
    return lastDecision;
}