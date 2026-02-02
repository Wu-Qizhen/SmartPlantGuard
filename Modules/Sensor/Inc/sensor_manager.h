#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "sensor_types.h"

// 传感器管理器状态
typedef struct {
    bool isInitialized;
    uint8_t readInterval;      // 读取间隔（秒）
    bool autoCalibration;      // 自动校准
    uint32_t totalReadCount;   // 总读取次数
    uint32_t errorCount;       // 错误计数
} SensorManagerStatus;

// 初始化传感器系统
SensorStatusEnum SensorManager_Init(void);

// 读取所有传感器数据
bool SensorManager_ReadAll(AllSensorData* sensorData);

// 读取单个传感器
SensorStatusEnum SensorManager_ReadSoilMoisture(float* moisture);
SensorStatusEnum SensorManager_ReadTemperature(float* temperature);
SensorStatusEnum SensorManager_ReadHumidity(float* humidity);
SensorStatusEnum SensorManager_ReadLightIntensity(float* light);

// 校准函数
bool SensorManager_CalibrateSoilMoisture(float dryValue, float wetValue);
bool SensorManager_CalibrateLightSensor(float minLux, float maxLux);

// 设置读取间隔
void SensorManager_SetReadInterval(uint8_t seconds);

// 获取管理器状态
SensorManagerStatus SensorManager_GetStatus(void);

// 重置传感器统计
void SensorManager_ResetStatistics(void);

#endif // SENSOR_MANAGER_H