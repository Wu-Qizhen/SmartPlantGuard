#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include "sensor_types.h"

// 土壤湿度校准结构
typedef struct {
    float dryValue;      // 干燥时的 ADC 值
    float wetValue;      // 湿润时的 ADC 值
    bool isCalibrated;   // 是否已校准
} SoilCalibration;

// 初始化土壤湿度传感器
bool SoilMoisture_Init(ADC_HandleTypeDef* hadc, uint32_t channel);

// 读取土壤湿度
SensorStatusEnum SoilMoisture_Read(float* moisturePercent);

// 校准函数
void SoilMoisture_CalibrateDry(void);  // 校准干燥值
void SoilMoisture_CalibrateWet(void);  // 校准湿润值
SoilCalibration SoilMoisture_GetCalibration(void);

// 设置自定义校准值
void SoilMoisture_SetCalibration(float dryValue, float wetValue);

#endif // SOIL_MOISTURE_H