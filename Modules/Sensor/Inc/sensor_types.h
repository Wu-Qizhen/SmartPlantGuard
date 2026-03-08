/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// 传感器数据单位
typedef enum {
    UNIT_PERCENT = 0, // 百分比
    UNIT_CELSIUS, // 摄氏度
    UNIT_LUX, // 勒克斯
    UNIT_RAW // 原始值
} SensorUnitEnum;

// 传感器状态
typedef enum {
    SENSOR_OK = 0,
    SENSOR_NOT_CONNECTED,
    SENSOR_OUT_OF_RANGE,
    SENSOR_TIMEOUT,
    SENSOR_ERROR
} SensorStatusEnum;

// 传感器数据包结构体
typedef struct {
    float value; // 传感器数值
    SensorUnitEnum unit; // 数值单位
    SensorStatusEnum status; // 传感器状态
    // uint32_t timestamp; // 时间戳
    // float minValue; // 最小值（用于校准）
    // float maxValue; // 最大值（用于校准）
} SensorData;

// 所有传感器数据集合
typedef struct {
    SensorData soilMoisture; // 土壤湿度（0-100%）
    SensorData temperature; // 温度（°C）
    SensorData humidity; // 湿度（%）
    SensorData lightIntensity; // 光照强度
    bool allSensorsValid; // 所有传感器是否有效
    uint32_t lastUpdateTime; // 最后更新时间
} AllSensorData;

// 编译时断言：确保 AllSensorData 大小不超过 128 字节
static_assert(sizeof(AllSensorData) <= 128, "AllSensorData too large for hardcoded buffer");

#pragma pack(push, 1) // 禁止填充，确保紧凑
typedef struct {
    uint16_t soilMoisture; // 土壤湿度，放大 10 倍（0-1000 表示 0.0%-100.0%）
    int16_t temperature; // 温度，放大 10 倍（-500-1500 表示 -50.0°C-150.0°C）
    uint16_t humidity; // 湿度，放大 10 倍（0-1000）
    uint16_t lightIntensity; // 光照强度
    uint8_t statusFlags; // 位标志，bit0=土壤有效，bit1=温度有效，bit2=湿度有效，bit3=光敏有效
    uint32_t timestamp; // 时间戳
} CompactSensorData;
#pragma pack(pop)

#endif // SENSOR_TYPES_H
