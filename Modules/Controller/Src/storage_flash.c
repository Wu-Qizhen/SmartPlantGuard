/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "storage_flash.h"
#include "stm32f1xx_hal.h"
#include "spi.h"
#include "system_config.h"
#include <string.h>

static SystemConfig systemConfig;
static bool isStorageInitialized = false;
static osMutexId_t flashBusyMutex = NULL;

// 存储初始化
bool StorageFlash_Init(void) {
    flashBusyMutex = osMutexNew(NULL);
    if (flashBusyMutex == NULL) {
        return false; // 互斥量创建失败
    }

    isStorageInitialized = true;
    return true;
}

// 保存配置到 Flash，存入第 0 号扇区
bool StorageFlash_SaveConfig(ControlParams *config) {
    if (!config || !isStorageInitialized) {
        return false;
    }

    // 保存到 Flash
    if (osMutexAcquire(flashBusyMutex, 1000) != osOK) {
        return false; // 获取互斥量失败（超时或其他错误）
    }
    bool success = true;

    // 1. 写使能
    uint8_t writeEnableCmd[] = {0x06};
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup; // 出错直接跳转解锁

    // 2. 扇区擦除
    uint8_t sectorEraseCmd[] = {0x20, 0x00, 0x00, 0x00}; // 擦除第 0 号扇区
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, sectorEraseCmd, 4, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(300); // 等待擦除

    // 3. 写使能
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;

    // 4. 页编程
    uint8_t Cmd[4] = {0x02, 0x00, 0x00, 0x00};
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, Cmd, 4, 100) != HAL_OK) { success = false; }
    // 只有在指令发送成功后才发送数据
    if (success) {
        if (HAL_SPI_Transmit(&hspi1, (uint8_t *) config, sizeof(SystemConfig), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(20);

    systemConfig.controlParams = *config;

    // 错误处理 / 正常退出统一入口：释放忙标志，返回状态
cleanup:
    osMutexRelease(flashBusyMutex);
    return success;
}

// 从 Flash 加载配置
bool StorageFlash_LoadConfig(ControlParams *config) {
    if (!config || !isStorageInitialized) {
        return false;
    }

    if (osMutexAcquire(flashBusyMutex, 1000) != osOK) {
        return false; // 获取互斥量失败（超时或其他错误）
    }
    bool success = true;

    SystemConfig tempConfig; // 定义一个临时变量用于校验
    uint8_t readDataCmd[] = {0x03, 0x00, 0x00, 0x00};
    uint8_t readBuffer[sizeof(SystemConfig)];

    // 从 Flash 读取
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, readDataCmd, 4, 100) != HAL_OK) { success = false; }
    if (success) {
        if (HAL_SPI_Receive(&hspi1, readBuffer, sizeof(readBuffer), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;

    memcpy(&tempConfig, readBuffer, sizeof(SystemConfig));

    // 验证数据
    if (tempConfig.magicNumber != MAGIC_NUMBER) {
        success = false; // 数据无效视为失败
        goto cleanup;
    }

    *config = tempConfig.controlParams;
    systemConfig = tempConfig;

cleanup:
    osMutexRelease(flashBusyMutex);
    return success;
}

// 保存数据到 Flash
// 调用前记得给结构体赋值
bool StorageFlash_SaveData(SensorDataSave *dataSave) {
    if (!dataSave || !isStorageInitialized) {
        return false;
    }

    // 保存到 Flash
    if (osMutexAcquire(flashBusyMutex, 1000) != osOK) {
        return false; // 获取互斥量失败（超时或其他错误）
    }
    bool success = true; // 假设成功
    // 1. 写使能
    uint8_t writeEnableCmd[] = {0x06};
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup; // 出错直接跳转解锁

    // 2.扇区擦除
    uint8_t sectorEraseCmd[] = {0x20, 0x00, 0x10, 0x00}; // 擦除第 1 号扇区
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, sectorEraseCmd, 4, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(300); // 等待擦除

    // 3. 写使能
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;

    // 4. 页编程
    uint8_t Cmd[4] = {0x02, 0x00, 0x10, 0x00};
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, Cmd, 4, 100) != HAL_OK) { success = false; }
    // 只有在指令发送成功后才发送数据
    if (success) {
        if (HAL_SPI_Transmit(&hspi1, (uint8_t *) dataSave, sizeof(SensorDataSave), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(20);

cleanup:
    osMutexRelease(flashBusyMutex);
    return success;
}

// 从 Flash 加载数据
// 想要获取数据就调用此函数后从结构体中获取
bool StorageFlash_LoadData(SensorDataSave *dataSave) {
    if (!dataSave || !isStorageInitialized) {
        return false;
    }

    if (osMutexAcquire(flashBusyMutex, 1000) != osOK) {
        return false; // 获取互斥量失败（超时或其他错误）
    }
    bool success = true;

    SensorDataSave tempDataSave; // 定义一个临时变量用于校验
    uint8_t readDataCmd[] = {0x03, 0x00, 0x10, 0x00};
    uint8_t readBuffer[sizeof(SensorDataSave)];

    // 从 Flash 读取
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, readDataCmd, 4, 100) != HAL_OK) { success = false; }
    if (success) {
        if (HAL_SPI_Receive(&hspi1, readBuffer, sizeof(readBuffer), 1000) != HAL_OK) {
            success = false;
        }
    }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;

    memcpy(&tempDataSave, readBuffer, sizeof(SensorDataSave));

    // 验证数据
    if (tempDataSave.magicNumber != MAGIC_NUMBER) {
        success = false; // 数据无效视为失败
        goto cleanup;
    }

    *dataSave = tempDataSave;

cleanup:
    osMutexRelease(flashBusyMutex);
    return success;
}

// 调用完记得恢复默认设置
bool StorageFlash_Erase(uint32_t sectorIndex) {
    if (!isStorageInitialized) {
        return false;
    }

    // 1. 边界检查：防止越界擦除
    if (sectorIndex >= TOTAL_SECTORS) {
        return false;
    }

    // 擦除 Flash 中的配置参数部分
    if (osMutexAcquire(flashBusyMutex, 1000) != osOK) {
        return false; // 获取互斥量失败（超时或其他错误）
    }
    bool success = true; // 假设成功

    // 2. 计算扇区起始地址
    // 地址 = 扇区号 * 4096
    uint32_t address = sectorIndex * SECTOR_SIZE;
    uint8_t addrBytes[3];
    addrBytes[0] = (address >> 16) & 0xFF; // 高 8 位
    addrBytes[1] = (address >> 8) & 0xFF; // 中 8 位
    addrBytes[2] = (address) & 0xFF; // 低 8 位

    // 1. 写使能
    uint8_t writeEnableCmd[] = {0x06};
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, writeEnableCmd, 1, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup; // 出错直接跳转解锁

    // 2. 扇区擦除
    uint8_t sectorEraseCmd[4];
    sectorEraseCmd[0] = 0x20;
    sectorEraseCmd[1] = addrBytes[0];
    sectorEraseCmd[2] = addrBytes[1];
    sectorEraseCmd[3] = addrBytes[2];

    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi1, sectorEraseCmd, 4, 100) != HAL_OK) { success = false; }
    HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
    if (!success) goto cleanup;
    osDelay(300); // 等待擦除

cleanup:
    osMutexRelease(flashBusyMutex);
    return success;
}
