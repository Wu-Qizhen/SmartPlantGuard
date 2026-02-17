/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
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
    delay_init();
    DHT11_Init(DHT11_PORT, DHT11_PIN);
    AdcSensors_Init(ADC_SENSOR_HANDLE);

    managerStatus.isInitialized = true;
    return SENSOR_OK;
}

// 读取所有传感器数据
bool SensorManager_ReadAll(AllSensorData *sensorData) {
    if (!managerStatus.isInitialized || !sensorData) {
        return false;
    }

    // 统一接口读取温湿度
    float temp = 0, hum = 0;
    SensorStatusEnum tempHumStatus = SensorManager_ReadTemperatureHumidity(&temp, &hum);
    sensorData->temperature.value = temp;
    sensorData->humidity.value = hum;
    sensorData->temperature.unit = UNIT_CELSIUS;
    sensorData->humidity.unit = UNIT_PERCENT;
    sensorData->temperature.status = tempHumStatus;
    sensorData->humidity.status = tempHumStatus;

    float soil = 0, light = 0;
    SensorStatusEnum adcStatus = AdcSensors_Read(&soil, &light);
    sensorData->soilMoisture.value = soil;
    sensorData->lightIntensity.value = light;
    sensorData->soilMoisture.unit = UNIT_PERCENT;
    sensorData->lightIntensity.unit = UNIT_LUX;
    sensorData->soilMoisture.status = adcStatus;
    sensorData->lightIntensity.status = adcStatus;

    sensorData->lastUpdateTime = HAL_GetTick();
    sensorData->allSensorsValid = (tempHumStatus == SENSOR_OK) && (adcStatus == SENSOR_OK);

    managerStatus.totalReadCount++;
    if (tempHumStatus != SENSOR_OK || adcStatus != SENSOR_OK) {
        managerStatus.errorCount++;
    }
    return true;
}

// 读取土壤湿度（单独接口）
SensorStatusEnum SensorManager_ReadSoilMoisture(float *moisture) {
    float light; // 占位，忽略
    return AdcSensors_Read(moisture, &light);
}

// 读取光照强度（单独接口）
SensorStatusEnum SensorManager_ReadLightIntensity(float *light) {
    float soil; // 占位，忽略
    return AdcSensors_Read(&soil, light);
}

// 读取土壤湿度与光照强度
SensorStatusEnum SensorManager_ReadSoilMoistureLightIntensity(float *moisture, float *light) {
    return AdcSensors_Read(moisture, light);
}

// 读取温度（单独接口）
SensorStatusEnum SensorManager_ReadTemperature(float *temperature) {
    float hum;
    return SensorManager_ReadTemperatureHumidity(temperature, &hum);
}

// 读取湿度（单独接口）
SensorStatusEnum SensorManager_ReadHumidity(float *humidity) {
    float temp;
    return SensorManager_ReadTemperatureHumidity(&temp, humidity);
}

// 读取温度和湿度
SensorStatusEnum SensorManager_ReadTemperatureHumidity(float *temperature, float *humidity) {
    DHT11_StatusEnum status = DHT11_Read(temperature, humidity);

    // 映射 DHT11 状态到通用传感器状态
    // TODO: 简化，去掉特定错误码、映射
    if (status == DHT11_OK) {
        return SENSOR_OK;
    } else if (status == DHT11_TIMEOUT_ERROR || status == DHT11_NO_RESPONSE) {
        return SENSOR_TIMEOUT;
    } else if (status == DHT11_CHECKSUM_ERROR) {
        return SENSOR_CHECKSUM_ERROR;
    } else {
        return SENSOR_NOT_CONNECTED;
    }
}

// 校准土壤湿度（调用合并模块的接口）
bool SensorManager_CalibrateSoilMoisture(float dryValue, float wetValue) {
    AdcSensors_SetSoilCalibration(dryValue, wetValue);
    return true;
}

// 校准光照传感器（调用合并模块的接口）
bool SensorManager_CalibrateLightSensor(float minLux, float maxLux) {
    AdcSensors_SetLightRange(minLux, maxLux);
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
