#include "soil_moisture.h"

static ADC_HandleTypeDef *soilAdc;
static uint32_t soilChannel;
static SoilCalibration calibration = {
    .dryValue = 4095.0f, // 默认干燥值（ADC 最大值）
    .wetValue = 0.0f, // 默认湿润值（ADC 最小值）
    .isCalibrated = false
};

// 初始化土壤湿度传感器
bool SoilMoisture_Init(ADC_HandleTypeDef *hadc, uint32_t channel) {
    soilAdc = hadc;
    soilChannel = channel;
    return true;
}

// 读取土壤湿度
SensorStatusEnum SoilMoisture_Read(float *moisturePercent) {
    if (!soilAdc || !moisturePercent) {
        return SENSOR_NOT_CONNECTED;
    }

    // 启动 ADC 转换
    HAL_ADC_Start(soilAdc);
    if (HAL_ADC_PollForConversion(soilAdc, 100) == HAL_OK) {
        uint32_t adcValue = HAL_ADC_GetValue(soilAdc);
        HAL_ADC_Stop(soilAdc);

        // 计算湿度百分比
        if (calibration.isCalibrated) {
            *moisturePercent = 100.0f - ((adcValue - calibration.wetValue) / (
                                             calibration.dryValue - calibration.wetValue) * 100.0f);
        } else {
            // 使用默认校准值
            *moisturePercent = 100.0f - ((adcValue / 4095.0f) * 100.0f);
        }

        // 限制在 0-100% 范围内
        if (*moisturePercent < 0.0f) {
            *moisturePercent = 0.0f;
        } else if (*moisturePercent > 100.0f) {
            *moisturePercent = 100.0f;
        }

        return SENSOR_OK;
    } else {
        HAL_ADC_Stop(soilAdc);
        return SENSOR_TIMEOUT;
    }
}

// 校准干燥值
void SoilMoisture_CalibrateDry(void) {
    // 读取当前 ADC 值作为干燥值
    HAL_ADC_Start(soilAdc);
    if (HAL_ADC_PollForConversion(soilAdc, 100) == HAL_OK) {
        calibration.dryValue = (float) HAL_ADC_GetValue(soilAdc);
        calibration.isCalibrated = true;
    }
    HAL_ADC_Stop(soilAdc);
}

// 校准湿润值
void SoilMoisture_CalibrateWet(void) {
    // 读取当前 ADC 值作为湿润值
    HAL_ADC_Start(soilAdc);
    if (HAL_ADC_PollForConversion(soilAdc, 100) == HAL_OK) {
        calibration.wetValue = (float) HAL_ADC_GetValue(soilAdc);
        calibration.isCalibrated = true;
    }
    HAL_ADC_Stop(soilAdc);
}

// 获取校准值
SoilCalibration SoilMoisture_GetCalibration(void) {
    return calibration;
}

// 设置自定义校准值
void SoilMoisture_SetCalibration(float dryValue, float wetValue) {
    calibration.dryValue = dryValue;
    calibration.wetValue = wetValue;
    calibration.isCalibrated = true;
}
