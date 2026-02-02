#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "sensor_types.h"

// 初始化光敏传感器
bool LightSensor_Init(ADC_HandleTypeDef* hadc, uint32_t channel);

// 读取光照强度
SensorStatusEnum LightSensor_Read(float* lightIntensity);

// 校准函数
void LightSensor_CalibrateMin(void);
void LightSensor_CalibrateMax(void);

// 设置校准值
void LightSensor_SetCalibration(float minValue, float maxValue);

#endif // LIGHT_SENSOR_H