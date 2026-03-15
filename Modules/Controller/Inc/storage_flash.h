/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef STORAGE_FLASH_H
#define STORAGE_FLASH_H

#include "controller_types.h"
#include "sensor_types.h"

#define MAGIC_NUMBER 0x55AA55AA
#define SECTOR_SIZE 4096
#define TOTAL_SECTORS 2048
// W25Q64 总容量 8MB = 8388608 字节
// 扇区大小 4KB = 4096 字节
// 总扇区数 = 8MB / 4KB = 2048 个扇区

// 存储控制参数的数据结构
typedef struct {
    ControlParams controlParams; // 控制参数
    uint32_t magicNumber; // 魔数验证，一般设为 0x55AA55AA
    // uint32_t crc32; // CRC 校验
    // uint32_t saveCount; // 保存次数
    // uint32_t lastSaveTime; // 最后保存时间
} SystemConfig;

// 存储传感器数据的数据结构
typedef struct {
    SensorDataPacket sensorDataPacket; // 控制参数
    uint32_t magicNumber; // 魔数验证，一般设为 0x55AA55AA
    // uint32_t crc32; // CRC 校验
    // uint32_t saveCount; // 保存次数
    // uint32_t lastSaveTime; // 最后保存时间
} SensorDataSave;

// 存储初始化
bool StorageFlash_Init(void);

// 保存配置到 Flash
bool StorageFlash_SaveConfig(ControlParams *config);

// 从 Flash 加载配置
bool StorageFlash_LoadConfig(ControlParams *config);

// 保存数据到 Flash
bool StorageFlash_SaveData(SensorDataSave *dataSave);

// 从 Flash 加载数据
bool StorageFlash_LoadData(SensorDataSave *dataSave);

// 擦除存储
bool StorageFlash_Erase(uint32_t sectorIndex);

#endif // STORAGE_FLASH_H
