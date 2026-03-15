/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "storage_flash.h"
#include <string.h>
#include "spi.h"
#include "stm32f1xx_hal.h"

static SystemConfig systemConfig;
static bool isStorageInitialized = false;
bool is_busy = false;
// W25Q64 总容量 64Mbit = 8MB = 8,388,608 字节
// 扇区大小 4KB = 4096 字节
// 总扇区数 = 8MB / 4KB = 2048 个扇区 (0 ~ 2047)
#define W25Q64_SECTOR_SIZE      4096
#define W25Q64_TOTAL_SECTORS    2048

//static uint32_t calculateCRC32(uint8_t *data, uint32_t length);

// 存储初始化
bool StorageFlash_Init(void) {
    // 加载默认配置
    systemConfig.controlParams.soilMoistureLow = 30.0f;
    systemConfig.controlParams.soilMoistureHigh = 40.0f;
    systemConfig.controlParams.temperatureHigh = 30.0f;
    systemConfig.controlParams.temperatureLow = 25.0f;
    systemConfig.controlParams.lightIntensityLow = 100.0f;
    systemConfig.controlParams.lightIntensityHigh = 500.0f;
    systemConfig.controlParams.pumpMinInterval = 60;
    systemConfig.controlParams.pumpMaxDuration = 30;

    systemConfig.magicNumber = 0x55AA55AA;
    /*systemConfig.crc32 = 0;
    systemConfig.saveCount = 0;
    systemConfig.lastSaveTime = HAL_GetTick();*/

    isStorageInitialized = true;
    return true;
}

// 保存配置到 Flash(W25Q64)，存入第0号扇区
bool StorageFlash_SaveConfig(SystemConfig *config) {
    if (!config || !isStorageInitialized || is_busy) {
        return false;
    }

    /*// 计算 CRC
    config->crc32 = calculateCRC32((uint8_t *) &config->controlParams, sizeof(ControlParams));
    config->saveCount++;
    config->lastSaveTime = HAL_GetTick();*/

    // 保存到 Flash(W25Q64)
    is_busy = true;
    bool success = true; // 假设成功
    // 1.写使能
    uint8_t writeEnableCmd[] = {0x06};
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup; // 出错直接跳转解锁

    // 2.扇区擦除
    uint8_t sectorEraseCmd[] = {0x20, 0x00, 0x00, 0x00}; // 擦除第0号扇区
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, sectorEraseCmd, 4, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(300); // 等待擦除

    // 3.写使能
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;

    // 4.页编程
    uint8_t Cmd[4] = {0x02, 0x00, 0x00, 0x00};
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, Cmd, 4, 100) != HAL_OK) { success = false; }
    // 只有在指令发送成功后才发送数据
    if (success) {
        if (HAL_SPI_Transmit(&hspi1, (uint8_t *) config, sizeof(SystemConfig), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(20);

    systemConfig = *config;

    // 错误处理/正常退出统一入口：释放忙标志，返回状态
cleanup:
    is_busy = false;
    return success;
}

// 从 Flash(W25Q64) 加载配置
bool StorageFlash_LoadConfig(SystemConfig *config) {
    if (!config || !isStorageInitialized || is_busy) {
        return false;
    }

    is_busy = true;
    bool success = true;

    SystemConfig tempConfig; // 定义一个临时变量用于校验
    uint8_t readDataCmd[] = {0x03, 0x00, 0x00, 0x00};
    uint8_t read_buffer[sizeof(SystemConfig)];

    // 从 Flash(W25Q64) 读取
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, readDataCmd, 4, 100) != HAL_OK) { success = false; }
    if (success) {
        if (HAL_SPI_Receive(&hspi1, read_buffer, sizeof(read_buffer), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;

    memcpy(&tempConfig, read_buffer, sizeof(SystemConfig));

    // 验证数据
    if (tempConfig.magicNumber != 0x55AA55AA) {
        success = false; // 数据无效视为失败
        goto cleanup;
    }

    /*// 验证 CRC
    uint32_t crc32 = calculateCRC32((uint8_t *) &systemConfig.controlParams, sizeof(ControlParams));
    if (crc32 != systemConfig.crc32) {
        return false;
    }*/

    *config = tempConfig;
    systemConfig = *config;

cleanup:
    is_busy = false;
    return success;
}

// 恢复默认设置
bool StorageFlash_RestoreDefaults(void) {
    if (!isStorageInitialized || is_busy) {
        return false;
    }

    // 恢复默认配置
    systemConfig.controlParams.soilMoistureLow = 30.0f;
    systemConfig.controlParams.soilMoistureHigh = 40.0f;
    systemConfig.controlParams.temperatureHigh = 30.0f;
    systemConfig.controlParams.temperatureLow = 25.0f;
    systemConfig.controlParams.lightIntensityLow = 100.0f;
    systemConfig.controlParams.lightIntensityHigh = 500.0f;
    systemConfig.controlParams.pumpMinInterval = 60;
    systemConfig.controlParams.pumpMaxDuration = 30;

    systemConfig.magicNumber = 0x55AA55AA;
    /*systemConfig.crc32 = calculateCRC32((uint8_t *) &systemConfig.controlParams, sizeof(ControlParams));
    systemConfig.saveCount = 0;
    systemConfig.lastSaveTime = HAL_GetTick();*/

    // 保存到 Flash(W25Q64)
    return StorageFlash_SaveConfig(&systemConfig);
}

// 保存数据到 Flash
// 调用前记得给结构体赋值
bool StorageFlash_SaveData(SensorDataSave *DataSave) {
    if (!DataSave || !isStorageInitialized || is_busy) {
        return false;
    }

    /*// 计算 CRC
    config->crc32 = calculateCRC32((uint8_t *) &config->controlParams, sizeof(ControlParams));
    config->saveCount++;
    config->lastSaveTime = HAL_GetTick();*/

    // 保存到 Flash(W25Q64)
    is_busy = true;
    bool success = true; // 假设成功
    // 1.写使能
    uint8_t writeEnableCmd[] = {0x06};
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup; // 出错直接跳转解锁

    // 2.扇区擦除
    uint8_t sectorEraseCmd[] = {0x20, 0x00, 0x10, 0x00}; // 擦除第1号扇区
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, sectorEraseCmd, 4, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(300); // 等待擦除

    // 3.写使能
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;

    // 4.页编程
    uint8_t Cmd[4] = {0x02, 0x00, 0x10, 0x00};
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, Cmd, 4, 100) != HAL_OK) { success = false; }
    // 只有在指令发送成功后才发送数据
    if (success) {
        if (HAL_SPI_Transmit(&hspi1, (uint8_t *) DataSave, sizeof(SensorDataSave), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(20);

    // 错误处理/正常退出统一入口：释放忙标志，返回状态
cleanup:
    is_busy = false;
    return success;
}

// 从 Flash 加载数据
// 想要获取数据就调用此函数后从结构体中获取
bool StorageFlash_LoadData(SensorDataSave *DataSave) {
    if (!DataSave || !isStorageInitialized || is_busy) {
        return false;
    }

    is_busy = true;
    bool success = true;

    SensorDataSave tempDataSave; // 定义一个临时变量用于校验
    uint8_t readDataCmd[] = {0x03, 0x00, 0x10, 0x00};
    uint8_t read_buffer[sizeof(SensorDataSave)];

    // 从 Flash(W25Q64) 读取
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, readDataCmd, 4, 100) != HAL_OK) { success = false; }
    if (success) {
        if (HAL_SPI_Receive(&hspi1, read_buffer, sizeof(read_buffer), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;

    memcpy(&tempDataSave, read_buffer, sizeof(SensorDataSave));

    // 验证数据
    if (tempDataSave.magicNumber != 0x55AA55AA) {
        success = false; // 数据无效视为失败
        goto cleanup;
    }

    /*// 验证 CRC
    uint32_t crc32 = calculateCRC32((uint8_t *) &systemConfig.controlParams, sizeof(ControlParams));
    if (crc32 != systemConfig.crc32) {
        return false;
    }*/

    *DataSave = tempDataSave;

cleanup:
    is_busy = false;
    return success;
}

/**
 * @brief 擦除指定的 Flash 扇区
 * @param sectorIndex 扇区索引 (0 ~ 2047)
 * @retval true: 成功, false: 失败
 */
// 调用完记得恢复默认设置
bool StorageFlash_Erase(uint32_t sectorIndex) {
    if (!isStorageInitialized || is_busy) {
        return false;
    }

    // 1. 边界检查：防止越界擦除
    if (sectorIndex >= W25Q64_TOTAL_SECTORS) {
        return false;
    }

    // 擦除 Flash(W25Q64)中的配置参数部分
    is_busy = true;
    bool success = true; // 假设成功

    // 2. 计算扇区起始地址 (3字节地址)
    // 地址 = 扇区号 * 4096
    uint32_t address = sectorIndex * W25Q64_SECTOR_SIZE;
    uint8_t addrBytes[3];
    addrBytes[0] = (address >> 16) & 0xFF; // 高8位
    addrBytes[1] = (address >> 8) & 0xFF; // 中8位
    addrBytes[2] = (address) & 0xFF; // 低8位

    // 1.写使能
    uint8_t writeEnableCmd[] = {0x06};
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup; // 出错直接跳转解锁

    // 2.扇区擦除
    uint8_t sectorEraseCmd[4];
    sectorEraseCmd[0] = 0x20;
    sectorEraseCmd[1] = addrBytes[0];
    sectorEraseCmd[2] = addrBytes[1];
    sectorEraseCmd[3] = addrBytes[2];

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, sectorEraseCmd, 4, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(300); // 等待擦除

cleanup:
    is_busy = false;
    return success;
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

    /*// 验证 CRC
    uint32_t crc32 = calculateCRC32((uint8_t *) &systemConfig.controlParams, sizeof(ControlParams));
    return (crc32 == systemConfig.crc32);*/

    return true;
}

/*// 获取保存次数
uint32_t StorageFlash_GetSaveCount(void) {
    if (!isStorageInitialized) {
        return 0;
    }

    //return systemConfig.saveCount;
}*/

/*暂时先不需要
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
}*/
