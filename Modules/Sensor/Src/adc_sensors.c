/**
 * 合并后的 ADC 传感器模块（土壤湿度 + 光敏）
 * 使用 ADC 扫描模式一次性读取两个通道，避免冲突
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.17
 */
#include "adc_sensors.h"

static ADC_HandleTypeDef *adcHandle; // 共享的 ADC 句柄

// 土壤湿度校准数据
static SoilCalibration soilCalib = {
    .dryValue = 4095.0f,
    .wetValue = 0.0f,
    .isCalibrated = false
};

// 光照强度校准数据
static LightCalibration lightCalib = {
    .minAdc = 0.0f,
    .maxAdc = 4095.0f,
    .minLux = 0.0f,
    .maxLux = 1000.0f, // 默认范围 0~1000 LUX
    .isCalibrated = false
};

// 内部辅助函数：执行 ADC 转换并返回两个通道的值（先 IN0 土壤，后 IN1 光敏）
static SensorStatusEnum readAdcValues(uint32_t *soilAdc, uint32_t *lightAdc) {
    if (!adcHandle || !soilAdc || !lightAdc) {
        return SENSOR_NOT_CONNECTED;
    }

    // 启动 ADC 转换（扫描模式会自动转换所有已配置的通道）
    HAL_ADC_Start(adcHandle);

    // 等待转换完成（两个通道全部转换完毕）
    if (HAL_ADC_PollForConversion(adcHandle, 100) != HAL_OK) {
        HAL_ADC_Stop(adcHandle);
        return SENSOR_TIMEOUT;
    }

    // 读取第一个通道（土壤湿度，IN0）
    *soilAdc = HAL_ADC_GetValue(adcHandle);

    // 等待第二个通道转换完成（通常第一个读取后自动触发下一个 EOC）
    if (HAL_ADC_PollForConversion(adcHandle, 100) != HAL_OK) {
        HAL_ADC_Stop(adcHandle);
        return SENSOR_TIMEOUT;
    }

    // 读取第二个通道（光敏，IN1）
    *lightAdc = HAL_ADC_GetValue(adcHandle);

    HAL_ADC_Stop(adcHandle);
    return SENSOR_OK;
}

// 初始化
bool AdcSensors_Init(ADC_HandleTypeDef *hadc) {
    adcHandle = hadc;
    return (adcHandle != NULL);
}

// 读取并计算两个传感器值
SensorStatusEnum AdcSensors_Read(float *soilMoisture, float *lightIntensity) {
    uint32_t soilAdc, lightAdc;
    SensorStatusEnum status = readAdcValues(&soilAdc, &lightAdc);
    if (status != SENSOR_OK) {
        return status;
    }

    // 土壤湿度计算
    if (soilCalib.isCalibrated) {
        // 使用校准值：湿度百分比 = 100 - ((adc - wet) / (dry - wet) * 100)
        *soilMoisture = 100.0f - ((float) soilAdc - soilCalib.wetValue) /
                        (soilCalib.dryValue - soilCalib.wetValue) * 100.0f;
    } else {
        // 默认：ADC 值越小越湿，假设 4095=0%，0=100%
        *soilMoisture = 100.0f - ((float) soilAdc / 4095.0f) * 100.0f;
    }

    // 限制在 0~100%
    if (*soilMoisture < 0.0f) *soilMoisture = 0.0f;
    if (*soilMoisture > 100.0f) *soilMoisture = 100.0f;

    // 光照强度计算
    // TODO: 光敏电阻可能是非线性的
    if (lightCalib.isCalibrated && (lightCalib.maxAdc > lightCalib.minAdc)) {
        // 线性映射到设置的 lux 范围
        *lightIntensity = lightCalib.minLux +
                          ((float) lightAdc - lightCalib.minAdc) /
                          (lightCalib.maxAdc - lightCalib.minAdc) *
                          (lightCalib.maxLux - lightCalib.minLux);
    } else {
        // 默认：假设 0~4095 对应 0~1000 lux
        *lightIntensity = ((float) lightAdc / 4095.0f) * 1000.0f;
    }

    // 限制在范围内（避免校准值异常导致越界）
    if (*lightIntensity < lightCalib.minLux) *lightIntensity = lightCalib.minLux;
    if (*lightIntensity > lightCalib.maxLux) *lightIntensity = lightCalib.maxLux;

    return SENSOR_OK;
}

// 土壤湿度校准接口
void AdcSensors_CalibrateSoilDry(void) {
    uint32_t soilAdc, lightAdc; // lightAdc 暂不使用
    if (readAdcValues(&soilAdc, &lightAdc) == SENSOR_OK) {
        soilCalib.dryValue = (float) soilAdc;
        soilCalib.isCalibrated = true;
    }
}

void AdcSensors_CalibrateSoilWet(void) {
    uint32_t soilAdc, lightAdc;
    if (readAdcValues(&soilAdc, &lightAdc) == SENSOR_OK) {
        soilCalib.wetValue = (float) soilAdc;
        soilCalib.isCalibrated = true;
    }
}

SoilCalibration AdcSensors_GetSoilCalibration(void) {
    return soilCalib;
}

void AdcSensors_SetSoilCalibration(float dryValue, float wetValue) {
    soilCalib.dryValue = dryValue;
    soilCalib.wetValue = wetValue;
    soilCalib.isCalibrated = true;
}

// 光照强度校准接口
void AdcSensors_CalibrateLightMin(void) {
    uint32_t soilAdc, lightAdc;
    if (readAdcValues(&soilAdc, &lightAdc) == SENSOR_OK) {
        lightCalib.minAdc = (float) lightAdc;
        lightCalib.isCalibrated = true;
    }
}

void AdcSensors_CalibrateLightMax(void) {
    uint32_t soilAdc, lightAdc;
    if (readAdcValues(&soilAdc, &lightAdc) == SENSOR_OK) {
        lightCalib.maxAdc = (float) lightAdc;
        lightCalib.isCalibrated = true;
    }
}

void AdcSensors_SetLightRange(float minLux, float maxLux) {
    lightCalib.minLux = minLux;
    lightCalib.maxLux = maxLux;
    // 注意：不改变 isCalibrated，因为 ADC 校准点未变
}

LightCalibration AdcSensors_GetLightCalibration(void) {
    return lightCalib;
}

void AdcSensors_SetLightCalibration(float minAdc, float maxAdc, float minLux, float maxLux) {
    lightCalib.minAdc = minAdc;
    lightCalib.maxAdc = maxAdc;
    lightCalib.minLux = minLux;
    lightCalib.maxLux = maxLux;
    lightCalib.isCalibrated = true;
}
