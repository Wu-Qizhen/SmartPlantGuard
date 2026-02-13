/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef STORAGE_FLASH_H
#define STORAGE_FLASH_H

#include "controller_types.h"

// 存储的数据结构
typedef struct {
    ControlParams controlParams; // 控制参数
    uint32_t magicNumber; // 魔数验证
    uint32_t crc32; // CRC 校验
    uint32_t saveCount; // 保存次数
    uint32_t lastSaveTime; // 最后保存时间
} SystemConfig;

// 存储初始化
bool StorageFlash_Init(void);

// 保存配置到 Flash
bool StorageFlash_SaveConfig(SystemConfig *config);

// 从 Flash 加载配置
bool StorageFlash_LoadConfig(SystemConfig *config);

// 恢复默认设置
bool StorageFlash_RestoreDefaults(void);

// 擦除存储
bool StorageFlash_Erase(void);

// 获取存储状态
bool StorageFlash_IsValid(void);

uint32_t StorageFlash_GetSaveCount(void);

#endif // STORAGE_FLASH_H
