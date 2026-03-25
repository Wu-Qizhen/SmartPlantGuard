/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "cmsis_os2.h"
#include "sensor_types.h"
#include "system_config.h"
#include "dht11.h"
#include "adc_sensors.h"

extern AllSensorData gLatestSensorData;
extern osMutexId_t gSensorDataMutex;

// 传感器管理器状态
typedef struct {
    bool isInitialized;
    uint8_t readInterval; // 读取间隔
    bool autoCalibration; // 自动校准
    uint32_t totalReadCount; // 总读取次数
    uint32_t errorCount; // 错误计数
} SensorManagerStatus;

/**
 * 初始化传感器系统
 * @return 初始化是否成功
 */
bool SensorManager_Init(void);

/**
 * 读取所有传感器数据
 * @param sensorData 传感器数据结构体
 * @return 读取是否成功
 */
bool SensorManager_ReadAll(AllSensorData *sensorData);

/**
 * 读取土壤湿度
 * @param moisture 土壤湿度值
 * @return 读取状态
 */
SensorStatusEnum SensorManager_ReadSoilMoisture(float *moisture);

/**
 * 读取光照强度
 * @param light 光照强度值
 * @return 读取状态
 */
SensorStatusEnum SensorManager_ReadLightIntensity(float *light);

/**
 * 同时读取土壤湿度和光照强度
 * @param moisture 土壤湿度值
 * @param light 光照强度值
 * @return 读取状态
 */
SensorStatusEnum SensorManager_ReadSoilMoistureLightIntensity(float *moisture, float *light);

/**
 * 读取温度
 * @param temperature 温度值
 * @return 读取状态
 */
SensorStatusEnum SensorManager_ReadTemperature(float *temperature);

/**
 * 读取湿度
 * @param humidity 湿度值
 * @return 读取状态
 */
SensorStatusEnum SensorManager_ReadHumidity(float *humidity);

/**
 * 同时读取温度和湿度
 * @param temperature 温度值
 * @param humidity 湿度值
 * @return 读取状态
 */
SensorStatusEnum SensorManager_ReadTemperatureHumidity(float *temperature, float *humidity);

/**
 * 校准土壤湿度传感器
 * @param dryValue 干燥值
 * @param wetValue 湿润值
 * @return 校准是否成功
 */
bool SensorManager_CalibrateSoilMoisture(float dryValue, float wetValue);

/**
 * 校准光照传感器
 * @param minLux 最小光照值
 * @param maxLux 最大光照值
 * @return 校准是否成功
 */
bool SensorManager_CalibrateLightSensor(float minLux, float maxLux);

/**
 * 设置读取间隔
 * @param seconds 读取间隔（秒）
 */
void SensorManager_SetReadInterval(uint8_t seconds);

/**
 * 获取管理器状态
 * @return 传感器管理器状态结构体
 */
SensorManagerStatus SensorManager_GetStatus(void);

/**
 * 重置传感器统计
 */
void SensorManager_ResetStatistics(void);

#endif // SENSOR_MANAGER_H
