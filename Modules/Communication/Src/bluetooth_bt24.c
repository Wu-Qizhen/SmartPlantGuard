/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "bluetooth_bt24.h"
#include "cmsis_os2.h"
#include "usart.h"
#include "system_config.h"
#include "stdio.h"
#include <string.h>

#define TX_BUFFER_SIZE 64
#define CMD_BUFFER_SIZE 64
#define AT_TIMEOUT_MS 1000

uint8_t commRxByte;

static UART_HandleTypeDef *bluetoothUart;
static BluetoothStatus btStatus = {
    .state = BT_STATE_DISCONNECTED,
    .isPaired = false,
    .bytesReceived = 0,
    .bytesSent = 0
};
static uint8_t txBuffer[TX_BUFFER_SIZE]; // 静态发送缓冲区
static uint16_t txLength; // 当前待发送长度
static osSemaphoreId_t txSemaphore; // 发送资源信号量

// 发送 AT 指令并等待响应
static bool sendATCommand(const char *cmd) {
    int len = snprintf((char *) txBuffer, sizeof(txBuffer), "%s\r\n", cmd);
    if (len <= 0) return false;

    // 发送命令
    HAL_StatusTypeDef status = HAL_UART_Transmit(bluetoothUart, (uint8_t *) txBuffer, len, AT_TIMEOUT_MS);
    if (status != HAL_OK) return false;

    // 轮询接收响应，直到收到 OK 或超时
    uint8_t rxBuff[MAX_DATA_SIZE];
    uint32_t tickstart = HAL_GetTick();
    uint16_t index = 0;
    bool okReceived = false;

    while ((HAL_GetTick() - tickstart) < AT_TIMEOUT_MS) {
        uint8_t byte;
        // 尝试接收一个字节
        if (HAL_UART_Receive(bluetoothUart, &byte, 1, 10) == HAL_OK) {
            if (index < sizeof(rxBuff) - 1) {
                rxBuff[index++] = byte;
            }
            // 检查是否收到 OK\r\n
            if (index >= 4 && rxBuff[index - 4] == 'O' && rxBuff[index - 3] == 'K' &&
                rxBuff[index - 2] == '\r' && rxBuff[index - 1] == '\n') {
                okReceived = true;
                break;
            }
        }
    }
    return okReceived;
}

// 更新蓝牙连接状态（读取 STATE 引脚）
void Bluetooth_UpdateState(void) {
    GPIO_PinState pinState = HAL_GPIO_ReadPin(BLUETOOTH_STATE_PORT, BLUETOOTH_STATE_PIN);
    if (pinState == GPIO_PIN_SET) {
        btStatus.state = BT_STATE_CONNECTED;
        btStatus.isPaired = true; // 连接通常意味着已配对
    } else {
        btStatus.state = BT_STATE_DISCONNECTED;
        btStatus.isPaired = false;
    }
}

/*// 初始化蓝牙模块（HC05）
bool Bluetooth_Init(UART_HandleTypeDef *huart, BluetoothConfig *config) {
    txSemaphore = osSemaphoreNew(1, 1, NULL); // 初始可用
    if (txSemaphore == NULL) return false;

    if (!huart || !config) {
        return false;
    }

    bluetoothUart = huart;

    // 初始化状态机
    btStatus.state = BT_STATE_DISCONNECTED;
    btStatus.isPaired = false;
    btStatus.bytesReceived = 0;
    btStatus.bytesSent = 0;

    // 确保初始退出 AT 模式
    HAL_GPIO_WritePin(BLUETOOTH_EN_PORT, BLUETOOTH_EN_PIN, GPIO_PIN_RESET);
    osDelay(100);

    // 进入 AT 模式：拉高 PB8
    HAL_GPIO_WritePin(BLUETOOTH_EN_PORT, BLUETOOTH_EN_PIN, GPIO_PIN_SET);
    osDelay(500); // 等待模块进入 AT 模式

    // 1. 测试 AT 指令
    if (!sendATCommand("AT")) {
        HAL_GPIO_WritePin(BLUETOOTH_EN_PORT, BLUETOOTH_EN_PIN, GPIO_PIN_RESET); // 退出 AT 模式
        return false;
    }

    // 2. 设置设备名称
    char cmd[CMD_BUFFER_SIZE];
    snprintf(cmd, sizeof(cmd), "AT+NAME=%s", config->deviceName);
    sendATCommand(cmd); // 忽略失败，继续尝试

    // 3. 设置配对码（需与 Android 端一致）
    snprintf(cmd, sizeof(cmd), "AT+PSWD=%s", config->pinCode);
    sendATCommand(cmd);

    // 4. 退出 AT 模式：拉低 PB8
    HAL_GPIO_WritePin(BLUETOOTH_EN_PORT, BLUETOOTH_EN_PIN, GPIO_PIN_RESET);
    osDelay(500); // 等待模块重启并进入正常工作模式

    return true;
}*/

// 初始化蓝牙模块（BT24）
bool Bluetooth_Init(UART_HandleTypeDef *huart, BluetoothConfig *config) {
    txSemaphore = osSemaphoreNew(1, 1, NULL); // 初始可用
    if (txSemaphore == NULL) return false;

    if (!huart || !config) {
        return false;
    }

    bluetoothUart = huart;

    // 初始读取一次状态
    Bluetooth_UpdateState();
    btStatus.bytesReceived = 0;
    btStatus.bytesSent = 0;

    // BT24 无需 GPIO 控制，直接进入 AT 命令模式（模块上电默认即为命令模式，前提是未被连接）

    // 1. 测试 AT 指令
    if (!sendATCommand("AT")) {
        return false; // 通信失败，直接返回
    }

    // 2. 设置设备名称（BT24 指令格式：AT+NAME<name>，无等号）
    char cmd[CMD_BUFFER_SIZE];
    snprintf(cmd, sizeof(cmd), "AT+NAME%s", config->deviceName);
    sendATCommand(cmd); // 忽略失败，继续尝试

    // 3. 设置配对码（BT24 指令格式：AT+PIN<code>，无等号）
    //    若 BT24 仅支持数字密码，而 config->pinCode 为字母，此命令可能失败，
    //    但不会影响整体初始化（可根据实际模块调整）
    snprintf(cmd, sizeof(cmd), "AT+PIN%s", config->pinCode);
    sendATCommand(cmd);

    // 4. 软件复位使配置生效（BT24 需要重启）
    sendATCommand("AT+RESET");
    osDelay(500); // 等待模块重启

    return true;
}

// 发送数据
bool Bluetooth_SendData(uint8_t *data, uint16_t length) {
    if (!data || !bluetoothUart || length == 0 || length > TX_BUFFER_SIZE) {
        return false;
    }

    // 首次检查状态
    if (btStatus.state != BT_STATE_CONNECTED) {
        // 刷新一次连接状态（读取硬件引脚）
        Bluetooth_UpdateState();
        // 再次检查
        if (btStatus.state != BT_STATE_CONNECTED) {
            return false; // 仍为未连接，发送失败
        }
    }

    // 等待前一次发送完成（超时 100ms，可根据需要调整）
    if (osSemaphoreAcquire(txSemaphore, 100) != osOK) {
        return false; // 发送资源忙
    }

    // 拷贝数据到静态缓冲区
    memcpy(txBuffer, data, length);
    txLength = length;

    // 启动中断发送
    if (HAL_UART_Transmit_IT(bluetoothUart, txBuffer, txLength) != HAL_OK) {
        osSemaphoreRelease(txSemaphore); // 启动失败，释放信号量
        return false;
    }

    // 统计字节数（注意：此时尚未发送完成，但可先累加，或放在回调中累加）
    btStatus.bytesSent += length; // 如果要求精确，应在回调中累加

    return true;
}

// 阻塞发送
bool Bluetooth_SendDataBlocking(uint8_t *data, uint16_t length, uint32_t timeout) {
    if (!Bluetooth_SendData(data, length)) return false;
    // 等待发送完成信号量（此时信号量已被 SendData 占用，等待释放）
    return (osSemaphoreAcquire(txSemaphore, timeout) == osOK);
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

/*// 获取状态
BluetoothStatus Bluetooth_GetStatus(void) {
    Bluetooth_UpdateState();
    return btStatus;
}*/

void Bluetooth_StartReceive() {
    HAL_UART_Receive_IT(bluetoothUart, &commRxByte, 1); // 初始化完成后开启中断
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == bluetoothUart) {
        // 将字节放入队列
        osMessageQueuePut(Queue_BluetoothRxHandle, &commRxByte, 0, 0);
        // 重新开启下一次接收
        HAL_UART_Receive_IT(huart, &commRxByte, 1);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == bluetoothUart) {
        // 发送完成，释放信号量，允许下一次发送
        osSemaphoreRelease(txSemaphore);
        // 如果之前未统计，在这里更新精确的已发送字节数
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart == bluetoothUart) {
        // 发生错误，释放信号量防止死锁，并可尝试恢复
        osSemaphoreRelease(txSemaphore);
        btStatus.state = BT_STATE_ERROR; // 发生错误时置为错误状态
        // 1. 发生错误时（如溢出 ORE、噪音 FE 等），UART 中断会停止
        // 2. 必须在这里重新开启接收中断，才能恢复通信
        HAL_UART_Receive_IT(huart, &commRxByte, 1);
    }
}

/*// 配置蓝牙设备名称
bool Bluetooth_SetDeviceName(const char *name) {
    if (!name) {
        return false;
    }

    return true;
}*/

/*// 配置蓝牙配对码
bool Bluetooth_SetPinCode(const char *pin) {
    if (!pin) {
        return false;
    }

    return true;
}*/
