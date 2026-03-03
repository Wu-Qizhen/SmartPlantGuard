/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "bluetooth_hc05.h"
#include <string.h>
#include "stdio.h"
// #include "flash_ops.h"  // 请取消注释并包含你的 Flash 操作头文件

// TODO: 开发调试开关位于.h文件中，换模式去那调

// [内部数据结构] (用于 Flash 存储)
typedef struct {
    uint32_t magic;        // 魔数校验
    uint8_t  isConfigured; // 1: 已配置, 0: 未配置
} SystemConfigFlag;

static UART_HandleTypeDef *bluetoothUart;
static BluetoothStatus btStatus = {
    .state = BT_STATE_DISCONNECTED,
    .isPaired = false,
    .bytesReceived = 0,
    .bytesSent = 0,
    .connectCount = 0
};

static uint8_t rxBuffer[64];
static uint16_t rxBufferIndex = 0;

// 内部辅助宏
#define CONFIG_MAGIC_NUMBER  0xA5A55AA5  //用于写入flag->magic，以便后续判断是否需要配置
#define AT_CMD_TIMEOUT      500
#define AT_RESPONSE_SIZE    64

/**
 * @brief 内部辅助函数：发送 AT 命令并等待 "OK" 响应
 * @param cmd: 要发送的命令字符串 (不含 \r\n)
 * @return true: 收到 OK, false: 超时或收到 ERROR
 */
static bool SendAT_WaitOK(const char *cmd) {
    uint8_t response[AT_RESPONSE_SIZE] = {0};
    uint32_t startTime = HAL_GetTick();
    uint8_t rxByte;
    uint8_t idx = 0;
    bool foundOK = false;

    // 1. 发送命令 + 回车换行
    if (HAL_UART_Transmit(bluetoothUart, (uint8_t*)cmd, strlen(cmd), AT_CMD_TIMEOUT) != HAL_OK) return false;
    if (HAL_UART_Transmit(bluetoothUart, (uint8_t*)"\r\n", 2, AT_CMD_TIMEOUT) != HAL_OK) return false;

    // 2. 循环接收响应，直到超时或收到完整行
    while (HAL_GetTick() - startTime < AT_CMD_TIMEOUT) {
        if (HAL_UART_Receive(bluetoothUart, &rxByte, 1, 10) == HAL_OK) {
            if (idx < AT_RESPONSE_SIZE - 1) {
                response[idx++] = rxByte;

                // 简单判断：如果接收到 "OK" 关键字
                if (strstr((char*)response, "OK") != NULL) {
                    foundOK = true;
                    break;
                }
                // 如果接收到 "ERROR"
                if (strstr((char*)response, "ERROR") != NULL) {
                    break;
                }
            }
        }
    }

    return foundOK;
}

/**
 * @brief 内部辅助函数：从 Flash 读取配置标志
 * ⚠️ 请根据实际硬件替换此函数实现
 */
static void Read_Flag_From_Flash(SystemConfigFlag *flag) {
    // TODO: 替换为真实的 Flash 读取逻辑
    // 示例：memcpy(flag, (void*)0x0801F000, sizeof(SystemConfigFlag));

    // 【临时模拟】编译通过用 —— 实际项目请删除以下两行，启用真实读取
    flag->magic = 0;
    flag->isConfigured = 0;
}

/**
 * @brief 内部辅助函数：将配置标志写入 Flash
 * ⚠️ 请根据实际硬件替换此函数实现
 */
static void Write_Flag_To_Flash(SystemConfigFlag *flag) {
    // TODO: 替换为真实的 Flash 写入逻辑（含擦除步骤）
    // 示例：
    // HAL_FLASH_Unlock();
    // FLASH_Erase_InitTypeDef eraseInit = {...};
    // HAL_FLASHEx_Erase(&eraseInit, ...);
    // HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, flag->magic);
    // HAL_FLASH_Lock();

    // 【临时模拟】编译通过用 —— 实际项目请删除以下一行，启用真实写入
    printf("[BT-Flash] Simulated Write: Magic=0x%08lX, Configured=%d\r\n", flag->magic, flag->isConfigured);
}

// 初始化蓝牙模块
// TODO: 调用Init前要创建一下第二个参数并赋予初始值（或使用Flash中的数据）
bool Bluetooth_Init(UART_HandleTypeDef *huart, BluetoothConfig *config) {
    SystemConfigFlag flag;
    char cmdBuffer[64];

    if (!huart || !config) {
        return false;
    }

    bluetoothUart = huart;
    // 初始化状态机
    btStatus.state = BT_STATE_CONNECTING;
    btStatus.isPaired = false;
    btStatus.bytesReceived = 0;
    btStatus.bytesSent = 0;
    btStatus.connectCount = 0;  // 可选：清零或保留累计值

    // 配置蓝牙模块
    // [步骤 0] 读取 Flash 标志位，判断是否需要配置
    Read_Flag_From_Flash(&flag);
    // 核心判断逻辑：
    if (DEBUG_FORCE_REINIT || flag.magic != CONFIG_MAGIC_NUMBER || flag.isConfigured == 0) {
        // --- 执行 AT 指令配置序列 ---
        // 1. 通信握手测试
        if (!SendAT_WaitOK("AT")) {
            btStatus.state = BT_STATE_ERROR;  //若握手失败，应检查接线或波特率
            return false;
        }
        // 2. 设置设备名称
        snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+NAME=%s", config->deviceName);
        if (!SendAT_WaitOK(cmdBuffer)) {
            btStatus.state = BT_STATE_ERROR;
            return false;
        }
        // 3. 设置配对密码
        snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+PSWD=%s", config->pinCode);
        if (!SendAT_WaitOK(cmdBuffer)) {
            btStatus.state = BT_STATE_ERROR;
            return false;
        }
        // 4. 设置波特率
        snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+UART=%lu,0,0", (unsigned long)config->baudRate);
        if (!SendAT_WaitOK(cmdBuffer)) {
            btStatus.state = BT_STATE_ERROR;
            return false;
        }
        // 5. 设置为从机模式
        if (!SendAT_WaitOK("AT+ROLE=0")) {
            btStatus.state = BT_STATE_ERROR;
            return false;
        }

        // --- 关键：是否写入 Flash？ ---
        #if (DEBUG_FORCE_REINIT == 0)  // 0为写入
                flag.magic = CONFIG_MAGIC_NUMBER;
                flag.isConfigured = 1;
                Write_Flag_To_Flash(&flag);
                //printf("[BT-Info] 配置已保存至 Flash.\r\n");
        #else
                //printf("[BT-Debug] 调试模式：未保存至 Flash，下次重启将重新配置.\r\n");
        #endif

    }
    else{
        // [跳过配置] 直接认为就绪
        // 快速检查模块是否存活
        if (!SendAT_WaitOK("AT")) {
            // 进入错误状态
            btStatus.state = BT_STATE_ERROR;
            return false;
        }
    }

    // 更新最终状态
    btStatus.state = BT_STATE_CONNECTED;
    btStatus.isPaired = true;
    btStatus.connectCount++;  // 这只能记到Init的次数

    return true;
}

// 发送数据
bool Bluetooth_SendData(uint8_t *data, uint16_t length) {
    if (!data || !bluetoothUart) {
        return false;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(
        bluetoothUart,
        data,
        length,
        1000
    );

    if (status == HAL_OK) {
        btStatus.bytesSent += length;
        return true;
    }

    return false;
}

// 发送命令包
bool Bluetooth_SendPacket(CommandPacket *packet) {
    if (!packet) {
        return false;
    }

    // 构建数据包
    uint8_t buffer[40];
    uint16_t length = 0;

    buffer[length++] = packet->startByte;
    buffer[length++] = packet->command;
    buffer[length++] = packet->dataLength;

    memcpy(&buffer[length], packet->data, packet->dataLength);
    length += packet->dataLength;

    buffer[length++] = packet->checksum;
    buffer[length++] = packet->endByte;

    // 发送数据包
    return Bluetooth_SendData(buffer, length);
}

// 接收处理（在串口中断中调用）
void Bluetooth_ReceiveByte(uint8_t byte) {
    if (rxBufferIndex < sizeof(rxBuffer)) {
        rxBuffer[rxBufferIndex++] = byte;
    }
}

// 处理接收到的数据
bool Bluetooth_ProcessReceivedData(void) {
    if (rxBufferIndex == 0) {
        return false;
    }

    // 解析数据包
    CommandPacket packet;
    if (Protocol_ParsePacket(rxBuffer, rxBufferIndex, &packet)) {
        // 处理命令
        Response response = Protocol_ProcessCommand(&packet);

        // 发送响应
        CommandPacket ackPacket = {
            .startByte = 0xAA,
            .command = CMD_ACK,
            .dataLength = response.dataLength,
            .endByte = 0x55
        };
        memcpy(ackPacket.data, response.data, response.dataLength);
        ackPacket.checksum = Protocol_CalculateChecksum(
            &ackPacket.command,
            1 + ackPacket.dataLength
        );

        Bluetooth_SendPacket(&ackPacket);

        // 清空接收缓冲区
        rxBufferIndex = 0;
        return true;
    }

    // 检查是否需要清空缓冲区
    if (rxBufferIndex >= sizeof(rxBuffer)) {
        rxBufferIndex = 0;
    }

    return false;
}

// 获取状态
BluetoothStatus Bluetooth_GetStatus(void) {
    return btStatus;
}

// 配置蓝牙设备名称
bool Bluetooth_SetDeviceName(const char *name) {
    if (!name) {
        return false;
    }

    // 发送 AT 指令设置设备名称
    char cmdBuffer[64];
    snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+NAME=%s", name);

    if (!SendAT_WaitOK(cmdBuffer)) {
        return false;
    }

    // TODO: 还需存到Flash中

    return true;
}

// 配置蓝牙配对码
bool Bluetooth_SetPinCode(const char *pin) {
    if (!pin) {
        return false;
    }

    // 发送 AT 指令设置配对码
    char cmdBuffer[64];
    snprintf(cmdBuffer, sizeof(cmdBuffer), "AT+PSWD=%s", pin);

    if (!SendAT_WaitOK(cmdBuffer)) {
        return false;
    }

    // TODO: 还需存到Flash中

    return true;
}
