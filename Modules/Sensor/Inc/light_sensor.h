/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "sensor_types.h"
#include "stm32f1xx_hal.h"

// 初始化光敏传感器
bool LightSensor_Init(ADC_HandleTypeDef *hadc, uint32_t channel);

// 读取光照强度
SensorStatusEnum LightSensor_Read(float *lightIntensity);

// 校准函数
void LightSensor_CalibrateMin(void);

void LightSensor_CalibrateMax(void);

// 设置校准值
void LightSensor_SetCalibration(float minValue, float maxValue);

#endif // LIGHT_SENSOR_H
