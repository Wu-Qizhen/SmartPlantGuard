/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include <string.h>
#include "bluetooth_bt24.h"
#include "usart.h"

#define RX_BUFFER_SIZE 64

// 初始化设备名称、配对密码、波特率
static BluetoothConfig btConfig = {
    .deviceName = "PlantGuard_5NV9",
    .pinCode = "McEnvCtr",
    .baudRate = 9600
};

// 手动构造数据
/*static void sendSensorDataPeriodically(void) {
    // 1. 构造模拟的传感器数据
    CompactSensorData mockData;
    mockData.soilMoisture = 456; // 45.6%
    mockData.temperature = 234; // 23.4°C
    mockData.humidity = 600; // 60.0%
    mockData.lightIntensity = 800; // 光照原始值
    mockData.statusFlags = 0x0F; // 全部传感器有效
    mockData.timestamp = HAL_GetTick() / 1000; // 模拟时间戳（秒）

    // 2. 构建响应包（符合协议：AA 09 [数据长度] [数据] [校验和] 55）
    CommandPacket ackPacket;
    ackPacket.startByte = 0xAA;
    ackPacket.command = CMD_ACK; // 0x09
    ackPacket.dataLength = sizeof(CompactSensorData); // 13 字节
    memcpy(ackPacket.data, &mockData, ackPacket.dataLength);
    ackPacket.checksum = Protocol_CalculateChecksum(
        &ackPacket.command, // 从命令字节开始
        2 + ackPacket.dataLength // 命令 + 数据长度 + 数据
    );
    ackPacket.endByte = 0x55;

    // 3. 发送数据包
    if (Bluetooth_SendPacket(&ackPacket)) {
        // 发送成功，可添加调试输出
    } else {
        // 发送失败，可添加错误处理
    }
}*/

// 手动读取数据
/*static void sendSensorDataPeriodically(void) {
    AllSensorData sensorData;

    // 直接调用传感器读取函数（需要确保线程安全）
    if (SensorManager_ReadAll(&sensorData)) {
        // 转换为紧凑格式（与协议定义一致）
        CompactSensorData compact;
        compact.soilMoisture = (uint16_t) (sensorData.soilMoisture.value * 10);
        compact.temperature = (int16_t) (sensorData.temperature.value * 10);
        compact.humidity = (uint16_t) (sensorData.humidity.value * 10);
        compact.lightIntensity = (uint16_t) sensorData.lightIntensity.value;
        compact.statusFlags = 0;
        if (sensorData.soilMoisture.status == SENSOR_OK) compact.statusFlags |= 0x01;
        if (sensorData.temperature.status == SENSOR_OK) compact.statusFlags |= 0x02;
        if (sensorData.humidity.status == SENSOR_OK) compact.statusFlags |= 0x04;
        if (sensorData.lightIntensity.status == SENSOR_OK) compact.statusFlags |= 0x08;
        compact.timestamp = sensorData.lastUpdateTime / 1000; // 转换为秒

        // 构建响应包（协议格式：AA 09 数据长度 数据 校验和 55）
        CommandPacket ackPacket = {
            .startByte = 0xAA,
            .command = CMD_ACK,
            .dataLength = sizeof(CompactSensorData),
            .endByte = 0x55
        };
        memcpy(ackPacket.data, &compact, ackPacket.dataLength);
        ackPacket.checksum = Protocol_CalculateChecksum(
            &ackPacket.command,
            2 + ackPacket.dataLength
        );

        // 发送数据包
        Bluetooth_SendPacket(&ackPacket);
    }
    // 如果读取失败，可以选择不发送或发送错误指示
}*/

// 手动读取队列
// TODO
/*static void sendSensorDataPeriodically(void) {
    AllSensorData sensorData;

    // 尝试从队列获取数据（不阻塞，立即返回）
    if (osMessageQueueGet(Queue_SensorDataHandle, &sensorData, NULL, 0) == osOK) {
        // 成功获取到数据，打包发送
        CompactSensorData compact;
        compact.soilMoisture = (uint16_t) (sensorData.soilMoisture.value * 10);
        compact.temperature = (int16_t) (sensorData.temperature.value * 10);
        compact.humidity = (uint16_t) (sensorData.humidity.value * 10);
        compact.lightIntensity = (uint16_t) sensorData.lightIntensity.value;
        compact.statusFlags = 0;
        if (sensorData.soilMoisture.status == SENSOR_OK) compact.statusFlags |= 0x01;
        if (sensorData.temperature.status == SENSOR_OK) compact.statusFlags |= 0x02;
        if (sensorData.humidity.status == SENSOR_OK) compact.statusFlags |= 0x04;
        if (sensorData.lightIntensity.status == SENSOR_OK) compact.statusFlags |= 0x08;
        compact.timestamp = sensorData.lastUpdateTime / 1000;

        CommandPacket ackPacket = {
            .startByte = 0xAA,
            .command = CMD_ACK,
            .dataLength = sizeof(CompactSensorData),
            .endByte = 0x55
        };
        memcpy(ackPacket.data, &compact, ackPacket.dataLength);
        ackPacket.checksum = Protocol_CalculateChecksum(&ackPacket.command, 2 + ackPacket.dataLength);
        Bluetooth_SendPacket(&ackPacket);
    } else {
        // 队列为空，发送一个简单提示（可选）
        uint8_t msg[] = "No sensor data";
        Bluetooth_SendData(msg, sizeof(msg) - 1);
    }
}*/

// 模拟触发
// TODO
/*static void sendSensorDataPeriodically(void) {
    // 构造一个假的 GET_SENSOR_DATA 命令包，仅用于触发 processGetSensorData
    CommandPacket fakePacket;
    fakePacket.command = CMD_GET_SENSOR_DATA;
    fakePacket.dataLength = 0; // 该命令无数据

    // 调用协议处理函数获取响应（实际会调用 processGetSensorData）
    Response response = Protocol_ProcessCommand(&fakePacket);

    if (response.success) {
        // 构建响应包（格式同蓝牙通信协议）
        CommandPacket ackPacket = {
            .startByte = 0xAA,
            .command = CMD_ACK,
            .dataLength = response.dataLength,
            .endByte = 0x55
        };
        memcpy(ackPacket.data, response.data, response.dataLength);
        ackPacket.checksum = Protocol_CalculateChecksum(
            &ackPacket.command, // 从命令字节开始计算
            2 + ackPacket.dataLength
        );

        // 通过蓝牙发送
        Bluetooth_SendPacket(&ackPacket);
    }
    // 如果失败（比如无数据），可根据需要处理（例如发送空包或不发送）
}*/

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
        // 从队列获取一个字节（阻塞等待）
        /*if (osMessageQueueGet(Queue_BluetoothRxHandle, &rxByte, NULL, portMAX_DELAY) == osOK) {
            // 将收到的字节原封不动发回
            if (!Bluetooth_SendData(&rxByte, 1)) {
                // 发送失败处理（可重试或记录错误）
            }
        }*/
        /*if (osMessageQueueGet(Queue_BluetoothRxHandle, &rxByte, NULL, portMAX_DELAY) == osOK) {
            // 将字节存入蓝牙接收缓冲区
            Bluetooth_ReceiveByte(rxByte);
            // 尝试解析并响应（如果收到完整命令包，会自动发送响应）
            Bluetooth_ProcessReceivedData();
        }*/

        // 尝试接收字节（带超时，兼顾周期性发送）
        if (osMessageQueueGet(Queue_BluetoothRxHandle, &rxByte, NULL, 100) == osOK) {
            // 字节存入解析缓冲区（简单循环缓冲区或滑动窗口）
            if (parseLen < sizeof(parseBuf)) {
                parseBuf[parseLen++] = rxByte;
            } else {
                memmove(parseBuf, parseBuf + 1, sizeof(parseBuf) - 1);
                parseBuf[sizeof(parseBuf) - 1] = rxByte;
            }

            // 尝试解析完整命令包
            if (Protocol_ParsePacket(parseBuf, parseLen, &cmdPkt)) {
                // 解析成功，处理命令
                resp = Protocol_ProcessCommand(&cmdPkt);
                sendResponse(&resp);

                // 从解析缓冲区移除已处理的数据
                uint16_t pktLen = 5 + cmdPkt.dataLength; // 帧总长度
                memmove(parseBuf, parseBuf + pktLen, parseLen - pktLen);
                parseLen -= pktLen;
            }
            // 若未解析出完整包，继续接收
        } else {
            // 超时，执行周期性任务（如发送传感器数据）
        }
    }
}
