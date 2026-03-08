/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "bluetooth_bt24.h"
#include "usart.h"
#include <string.h>

#define RX_BUFFER_SIZE 64
#define MIN_PACKET_SIZE 5 // 最小包长度

// 初始化设备名称、配对密码、波特率
static BluetoothConfig btConfig = {
    .deviceName = "PlantGuard_5NV9",
    .pinCode = "McEnvCtr",
    .baudRate = 9600
};

static void sendResponse(const Response *resp) {
    CommandPacket ackPkt = {
        .startByte = 0xAA,
        .command = resp->success ? CMD_ACK : CMD_ERROR,
        .dataLength = resp->dataLength,
        .endByte = 0x55
    };
    memcpy(ackPkt.data, resp->data, resp->dataLength);
    ackPkt.checksum = Protocol_CalculateChecksum(&ackPkt.command, 2 + ackPkt.dataLength);
    Bluetooth_SendPacket(&ackPkt);
}

void StartTask_Comm(void *argument) {
    uint8_t rxByte;
    uint8_t parseBuf[RX_BUFFER_SIZE];
    uint16_t parseLen = 0;
    CommandPacket cmdPkt;
    Response resp;

    Bluetooth_Init(&huart1, &btConfig);
    Bluetooth_StartReceive(); // 初始化完成后开启中断

    for (;;) {
        // 尝试从消息队列接收一个字节
        if (osMessageQueueGet(Queue_BluetoothRxHandle, &rxByte, NULL, 100) == osOK) {
            // 将字节存入解析缓冲区（简单环形处理：若满则丢弃最早字节）
            if (parseLen < sizeof(parseBuf)) {
                parseBuf[parseLen++] = rxByte;
            } else {
                // 缓冲区已满，丢弃最早一个字节，再添加新字节
                memmove(parseBuf, parseBuf + 1, sizeof(parseBuf) - 1);
                parseBuf[sizeof(parseBuf) - 1] = rxByte;
                // 注意：parseLen 不变，仍为 sizeof(parseBuf)
            }

            // 循环解析：只要缓冲区长度 ≥ 最小包长，就尝试解析
            while (parseLen >= MIN_PACKET_SIZE) {
                if (Protocol_ParsePacket(parseBuf, parseLen, &cmdPkt)) {
                    // 解析成功，处理命令
                    resp = Protocol_ProcessCommand(&cmdPkt);
                    sendResponse(&resp);

                    // 从缓冲区移除已处理的完整包
                    uint16_t pktLen = 5 + cmdPkt.dataLength; // 总长度
                    if (pktLen <= parseLen) {
                        memmove(parseBuf, parseBuf + pktLen, parseLen - pktLen);
                        parseLen -= pktLen;
                    } else {
                        // 不应该发生，但以防万一
                        parseLen = 0;
                    }
                    // 继续循环，尝试解析剩余数据中可能存在的下一个包
                } else {
                    // 解析失败：丢弃第一个字节，重新尝试
                    memmove(parseBuf, parseBuf + 1, parseLen - 1);
                    parseLen--;
                }
            }
        } else {
            // 超时未收到数据，可执行周期性任务（如主动上报传感器数据）
        }
    }
}
