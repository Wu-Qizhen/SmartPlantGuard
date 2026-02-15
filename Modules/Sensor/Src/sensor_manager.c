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
    // 这里需要调用各个传感器的初始化函数
    delay_init();
    DHT11_Init(DHT11_PORT, DHT11_PIN);

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

    // 读取其他传感器
    SensorManager_ReadSoilMoisture(&sensorData->soilMoisture.value);
    SensorManager_ReadLightIntensity(&sensorData->lightIntensity.value);

    sensorData->soilMoisture.unit = UNIT_PERCENT;
    sensorData->lightIntensity.unit = UNIT_LUX;

    sensorData->lastUpdateTime = HAL_GetTick();
    sensorData->allSensorsValid = (tempHumStatus == SENSOR_OK); // TODO

    managerStatus.totalReadCount++;
    if (tempHumStatus != SENSOR_OK) {
        managerStatus.errorCount++;
    }
    return true;
}

// 读取单个传感器
SensorStatusEnum SensorManager_ReadSoilMoisture(float *moisture) {
    // 调用土壤湿度传感器的读取函数
    return SENSOR_OK;
}

SensorStatusEnum SensorManager_ReadTemperatureHumidity(float *temperature, float *humidity) {
    DHT11_StatusEnum status = DHT11_Read(temperature, humidity);

    // 映射 DHT11 状态到通用传感器状态
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

SensorStatusEnum SensorManager_ReadLightIntensity(float *light) {
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
