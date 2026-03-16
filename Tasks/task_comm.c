/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "usart.h"
#include "bluetooth_bt24.h"
#include "protocol.h"
#include <string.h>

#define RX_BUFFER_SIZE 64
#define RX_TIMEOUT_MS 500

static void sendResponse(const Response *resp) {
    CommandPacket ackPkt = {
        .startByte = PACKET_HEAD,
        .command = resp->success ? CMD_ACK : CMD_ERROR,
        .dataLength = resp->dataLength,
        .endByte = PACKET_TAIL
    };
    memcpy(ackPkt.data, resp->data, resp->dataLength);
    ackPkt.checksum = Protocol_CalculateChecksum(&ackPkt.command, 2 + ackPkt.dataLength);
    Bluetooth_SendPacket(&ackPkt);
}

void StartTask_Comm(void *argument) {
    uint8_t rxByte;
    uint8_t parseBuf[RX_BUFFER_SIZE];
    uint16_t parseLen = 0;
    uint32_t lastRxTick = HAL_GetTick(); // 最后收到数据的时间戳

    Bluetooth_Init(&huart1);
    Bluetooth_StartReceive();

    for (;;) {
        // 全局接收超时检
        if (parseLen > 0 && (HAL_GetTick() - lastRxTick) > RX_TIMEOUT_MS) {
            // 长时间未收到新数据，认为当前不完整包已超时，清空缓冲区
            parseLen = 0;
        }

        // 尝试从消息队列接收一个字节（超时 100ms）
        if (osMessageQueueGet(Queue_BluetoothRxHandle, &rxByte, NULL, 100) == osOK) {
            lastRxTick = HAL_GetTick(); // 更新最后接收时间

            // 将字节存入缓冲区（线性缓冲区 + memmove，简单处理）
            if (parseLen < sizeof(parseBuf)) {
                parseBuf[parseLen++] = rxByte;
            } else {
                // 缓冲区已满，丢弃最早一个字节，再添加新字节
                memmove(parseBuf, parseBuf + 1, sizeof(parseBuf) - 1);
                parseBuf[sizeof(parseBuf) - 1] = rxByte;
                // parseLen 保持最大值
            }

            // 循环解析缓冲区中的所有可能包
            uint16_t offset = 0;
            while (offset + MIN_PACKET_SIZE <= parseLen) {
                // 查找起始字节
                if (parseBuf[offset] != PACKET_HEAD) {
                    offset++;
                    continue;
                }

                // 确保能读取命令和数据长度字段
                if (offset + 3 > parseLen) break; // 需要命令和长度字段

                uint8_t cmd = parseBuf[offset + 1];
                uint8_t dataLen = parseBuf[offset + 2];

                // 命令字节合法性检查
                // NOTE: 若协议增加，这里需同步修改
                if (cmd < CMD_GET_SENSOR_DATA || cmd > CMD_SET_CONTROL_MODE) {
                    offset++; // 跳过非法命令
                    continue;
                }

                // 数据长度合理性检查（防止超大长度导致阻塞）
                if (dataLen > MAX_DATA_SIZE) {
                    // 长度超限，视为无效包，跳过当前起始字节
                    offset++;
                    continue;
                }

                uint16_t pktLen = 5 + dataLen; // 完整包长度

                // 检查缓冲区中是否有完整包
                if (offset + pktLen > parseLen) break; // 数据不足，等待更多数据

                // 尝试解析该包
                CommandPacket cmdPkt;
                if (Protocol_ParsePacket(&parseBuf[offset], pktLen, &cmdPkt)) {
                    // 解析成功，处理命令
                    Response resp = Protocol_ProcessCommand(&cmdPkt);
                    sendResponse(&resp);

                    // 移除已处理的包
                    memmove(parseBuf + offset, parseBuf + offset + pktLen, parseLen - offset - pktLen);
                    parseLen -= pktLen;
                    // 继续从同一 offset 检查剩余数据（可能还有包）
                } else {
                    // 解析失败（校验和错误等），跳过当前起始字节
                    offset++;
                }
            }

            // 清理已处理的无用前缀（将剩余数据移至缓冲区头部）
            if (offset > 0 && offset < parseLen) {
                memmove(parseBuf, parseBuf + offset, parseLen - offset);
                parseLen -= offset;
            } else if (offset >= parseLen) {
                parseLen = 0;
            }
        } else {
            // 超时未收到数据，可执行周期性任务（如主动上报传感器数据）
            // 此处留空，可根据需要添加
        }
    }
}
