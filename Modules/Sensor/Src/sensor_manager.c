#include "sensor_manager.h"

static SensorManagerStatus managerStatus = {
    .isInitialized = false,
    .readInterval = 5,
    .autoCalibration = false,
    .totalReadCount = 0,
    .errorCount = 0
};

// 初始化传感器系统
SensorStatusEnum SensorManager_Init(void) {
    // 初始化各个传感器
    // 这里需要调用各个传感器的初始化函数
    
    managerStatus.isInitialized = true;
    return SENSOR_OK;
}

// 读取所有传感器数据
bool SensorManager_ReadAll(AllSensorData* sensorData) {
    if (!managerStatus.isInitialized || !sensorData) {
        return false;
    }
    
    // 读取各个传感器数据
    SensorManager_ReadSoilMoisture(&sensorData->soilMoisture.value);
    SensorManager_ReadTemperature(&sensorData->temperature.value);
    SensorManager_ReadHumidity(&sensorData->humidity.value);
    SensorManager_ReadLightIntensity(&sensorData->lightIntensity.value);
    
    // 设置时间戳
    sensorData->lastUpdateTime = HAL_GetTick();
    sensorData->allSensorsValid = true; // 简化处理，实际需要检查各个传感器状态
    
    managerStatus.totalReadCount++;
    return true;
}

// 读取单个传感器
SensorStatusEnum SensorManager_ReadSoilMoisture(float* moisture) {
    // 调用土壤湿度传感器的读取函数
    return SENSOR_OK;
}

SensorStatusEnum SensorManager_ReadTemperature(float* temperature) {
    // 调用 DHT11 温度读取函数
    return SENSOR_OK;
}

SensorStatusEnum SensorManager_ReadHumidity(float* humidity) {
    // 调用 DHT11 湿度读取函数
    return SENSOR_OK;
}

SensorStatusEnum SensorManager_ReadLightIntensity(float* light) {
    // 调用光敏传感器的读取函数
    return SENSOR_OK;
}

// 校准函数
bool SensorManager_CalibrateSoilMoisture(float dryValue, float wetValue) {
    // 调用土壤湿度传感器的校准函数
    return true;
}

bool SensorManager_CalibrateLightSensor(float minLux, float maxLux) {
    // 调用光敏传感器的校准函数
    return true;
}

// 设置读取间隔
void SensorManager_SetReadInterval(uint8_t seconds) {
    managerStatus.readInterval = seconds;
}

// 获取管理器状态
SensorManagerStatus SensorManager_GetStatus(void) {
    return managerStatus;
}

// 重置传感器统计
void SensorManager_ResetStatistics(void) {
    managerStatus.totalReadCount = 0;
    managerStatus.errorCount = 0;
}