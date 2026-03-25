/**
 * 合并后的 ADC 传感器模块（土壤湿度 + 光敏）
 * 使用 ADC 扫描模式一次性读取两个通道，避免冲突
 *
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.17
 */
#ifndef ADC_SENSORS_H
#define ADC_SENSORS_H

#include "sensor_types.h"
#include "stm32f1xx_hal.h"

/* 土壤湿度校准参数 */
typedef struct {
    float dryValue; // 干燥时的 ADC 原始值
    float wetValue; // 湿润时的 ADC 原始值
    bool isCalibrated; // 是否已校准
} SoilCalibration;

/* 光照强度校准参数 */
typedef struct {
    float minAdc; // 最暗时的 ADC 原始值
    float maxAdc; // 最亮时的 ADC 原始值
    float minLux; // 最暗对应的物理光照值
    float maxLux; // 最亮对应的物理光照值
    bool isCalibrated; // 是否已校准
} LightCalibration;

/**
 * 初始化 ADC 传感器模块
 * @param hadc 已配置好的 ADC 句柄（IN0 土壤，IN1 光敏）
 * @return 初始化是否成功
 */
bool AdcSensors_Init(ADC_HandleTypeDef *hadc);

/**
 * 读取两个传感器的值
 * @param soilMoisture 返回土壤湿度百分比
 * @param lightIntensity 返回光照强度
 * @return 传感器状态
 */
SensorStatusEnum AdcSensors_Read(float *soilMoisture, float *lightIntensity);

/**
 * 土壤湿度校准 - 校准干燥值（传感器置于干燥空气中）
 */
void AdcSensors_CalibrateSoilDry(void);

/**
 * 土壤湿度校准 - 校准湿润值（传感器插入水中）
 */
void AdcSensors_CalibrateSoilWet(void);

/**
 * 设置土壤湿度校准值
 * @param dryValue 干燥时的 ADC 原始值
 * @param wetValue 湿润时的 ADC 原始值
 */
void AdcSensors_SetSoilCalibration(float dryValue, float wetValue);

/**
 * 获取土壤湿度校准参数
 * @return 土壤湿度校准结构体
 */
SoilCalibration AdcSensors_GetSoilCalibration(void);

/**
 * 光照强度校准 - 校准最小值（遮挡传感器，最暗）
 */
void AdcSensors_CalibrateLightMin(void);

/**
 * 光照强度校准 - 校准最大值（强光直射，最亮）
 */
void AdcSensors_CalibrateLightMax(void);

/**
 * 设置光照强度物理范围（默认 0~1000）
 * @param minLux 最暗对应的物理光照值
 * @param maxLux 最亮对应的物理光照值
 */
void AdcSensors_SetLightRange(float minLux, float maxLux);

/**
 * 设置光照强度校准值
 * @param minAdc 最暗时的 ADC 原始值
 * @param maxAdc 最亮时的 ADC 原始值
 * @param minLux 最暗对应的物理光照值
 * @param maxLux 最亮对应的物理光照值
 */
void AdcSensors_SetLightCalibration(float minAdc, float maxAdc, float minLux, float maxLux);

/**
 * 获取光照强度校准参数
 * @return 光照强度校准结构体
 */
LightCalibration AdcSensors_GetLightCalibration(void);

#endif // ADC_SENSORS_H
