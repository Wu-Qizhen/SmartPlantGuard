#include "light_sensor.h"

static ADC_HandleTypeDef* lightAdc;
static uint32_t lightChannel;
static float minValue = 0.0f;
static float maxValue = 4095.0f;

// 初始化光敏传感器
bool LightSensor_Init(ADC_HandleTypeDef* hadc, uint32_t channel) {
    lightAdc = hadc;
    lightChannel = channel;
    return true;
}

// 读取光照强度
SensorStatusEnum LightSensor_Read(float* lightIntensity) {
    if (!lightAdc || !lightIntensity) {
        return SENSOR_NOT_CONNECTED;
    }
    
    // 启动 ADC 转换
    HAL_ADC_Start(lightAdc);
    if (HAL_ADC_PollForConversion(lightAdc, 100) == HAL_OK) {
        uint32_t adcValue = HAL_ADC_GetValue(lightAdc);
        HAL_ADC_Stop(lightAdc);
        
        // 计算光照强度（简化处理，实际需要根据传感器特性校准）
        *lightIntensity = (adcValue / 4095.0f) * 1000.0f; // 假设最大 1000 lux
        
        return SENSOR_OK;
    } else {
        HAL_ADC_Stop(lightAdc);
        return SENSOR_TIMEOUT;
    }
}

// 校准最小值
void LightSensor_CalibrateMin(void) {
    // 读取当前 ADC 值作为最小值
    HAL_ADC_Start(lightAdc);
    if (HAL_ADC_PollForConversion(lightAdc, 100) == HAL_OK) {
        minValue = (float)HAL_ADC_GetValue(lightAdc);
    }
    HAL_ADC_Stop(lightAdc);
}

// 校准最大值
void LightSensor_CalibrateMax(void) {
    // 读取当前 ADC 值作为最大值
    HAL_ADC_Start(lightAdc);
    if (HAL_ADC_PollForConversion(lightAdc, 100) == HAL_OK) {
        maxValue = (float)HAL_ADC_GetValue(lightAdc);
    }
    HAL_ADC_Stop(lightAdc);
}

// 设置校准值
void LightSensor_SetCalibration(float minVal, float maxVal) {
    minValue = minVal;
    maxValue = maxVal;
}