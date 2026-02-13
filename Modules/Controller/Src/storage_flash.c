/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "storage_flash.h"
#include "stm32f1xx_hal.h"

static SystemConfig systemConfig;
static bool isStorageInitialized = false;

static uint32_t calculateCRC32(uint8_t *data, uint32_t length);

// 存储初始化
bool StorageFlash_Init(void) {
    // 初始化存储
    // 这里需要根据实际的 STM32 型号实现 Flash 操作

    // 加载默认配置
    systemConfig.controlParams.soilMoistureLow = 30.0f;
    systemConfig.controlParams.soilMoistureHigh = 40.0f;
    systemConfig.controlParams.temperatureHigh = 30.0f;
    systemConfig.controlParams.temperatureLow = 25.0f;
    systemConfig.controlParams.lightIntensityLow = 100.0f;
    systemConfig.controlParams.lightIntensityHigh = 500.0f;
    systemConfig.controlParams.hysteresisBand = 2.0f;
    systemConfig.controlParams.minPumpInterval = 60;
    systemConfig.controlParams.maxPumpDuration = 30;

    systemConfig.magicNumber = 0x55AA55AA;
    systemConfig.crc32 = 0;
    systemConfig.saveCount = 0;
    systemConfig.lastSaveTime = HAL_GetTick();

    isStorageInitialized = true;
    return true;
}

// 保存配置到 Flash
bool StorageFlash_SaveConfig(SystemConfig *config) {
    if (!config || !isStorageInitialized) {
        return false;
    }

    // 计算 CRC
    config->crc32 = calculateCRC32((uint8_t *) &config->controlParams, sizeof(ControlParams));
    config->saveCount++;
    config->lastSaveTime = HAL_GetTick();

    // 保存到 Flash
    // 这里需要根据实际的 STM32 型号实现 Flash 写入

    systemConfig = *config;
    return true;
}

// 从 Flash 加载配置
bool StorageFlash_LoadConfig(SystemConfig *config) {
    if (!config || !isStorageInitialized) {
        return false;
    }

    // 从 Flash 读取
    // 这里需要根据实际的 STM32 型号实现 Flash 读取

    // 验证数据
    if (systemConfig.magicNumber != 0x55AA55AA) {
        return false;
    }

    // 验证 CRC
    uint32_t crc32 = calculateCRC32((uint8_t *) &systemConfig.controlParams, sizeof(ControlParams));
    if (crc32 != systemConfig.crc32) {
        return false;
    }

    *config = systemConfig;
    return true;
}

// 恢复默认设置
bool StorageFlash_RestoreDefaults(void) {
    if (!isStorageInitialized) {
        return false;
    }

    // 恢复默认配置
    systemConfig.controlParams.soilMoistureLow = 30.0f;
    systemConfig.controlParams.soilMoistureHigh = 40.0f;
    systemConfig.controlParams.temperatureHigh = 30.0f;
    systemConfig.controlParams.temperatureLow = 25.0f;
    systemConfig.controlParams.lightIntensityLow = 100.0f;
    systemConfig.controlParams.lightIntensityHigh = 500.0f;
    systemConfig.controlParams.hysteresisBand = 2.0f;
    systemConfig.controlParams.minPumpInterval = 60;
    systemConfig.controlParams.maxPumpDuration = 30;

    systemConfig.magicNumber = 0x55AA55AA;
    systemConfig.crc32 = calculateCRC32((uint8_t *) &systemConfig.controlParams, sizeof(ControlParams));
    systemConfig.saveCount = 0;
    systemConfig.lastSaveTime = HAL_GetTick();

    // 保存到 Flash
    return StorageFlash_SaveConfig(&systemConfig);
}

// 擦除存储
bool StorageFlash_Erase(void) {
    if (!isStorageInitialized) {
        return false;
    }

    // 擦除 Flash
    // 这里需要根据实际的 STM32 型号实现 Flash 擦除

    return true;
}

// 获取存储状态
bool StorageFlash_IsValid(void) {
    if (!isStorageInitialized) {
        return false;
    }

    // 验证数据
    if (systemConfig.magicNumber != 0x55AA55AA) {
        return false;
    }

    // 验证 CRC
    uint32_t crc32 = calculateCRC32((uint8_t *) &systemConfig.controlParams, sizeof(ControlParams));
    return (crc32 == systemConfig.crc32);
}

// 获取保存次数
uint32_t StorageFlash_GetSaveCount(void) {
    if (!isStorageInitialized) {
        return 0;
    }

    return systemConfig.saveCount;
}

// 计算 CRC32
static uint32_t calculateCRC32(uint8_t *data, uint32_t length) {
    // 简化的 CRC32 计算
    // 实际应用中需要使用标准的 CRC32 算法
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc ^ 0xFFFFFFFF;
}
