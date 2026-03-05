/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#include "protocol.h"
#include <string.h>
#include "sensor_manager.h"
#include "actuator_manager.h"
#include "controller_core.h"

static void processGetSensorData(Response *response);

static void processGetActuatorState(Response *response);

static void processSetActuator(Response *response, const CommandPacket *packet);

static void processSetParams(Response *response, const CommandPacket *packet);

static void processGetParams(Response *response);

static void processReset(Response *response);

static void processCalibrate(Response *response, CommandPacket *packet);

static void processGetSystemInfo(Response *response);

// 解析数据包
bool Protocol_ParsePacket(uint8_t *buffer, uint16_t length, CommandPacket *packet) {
    if (!buffer || !packet || length < 6) {
        return false;
    }

    // 检查起始字节和结束字节
    if (buffer[0] != 0xAA || buffer[length - 1] != 0x55) {
        return false;
    }

    // 解析数据包
    packet->startByte = buffer[0];
    packet->command = buffer[1];
    packet->dataLength = buffer[2];

    // 检查数据长度
    // 帧的固定部分包括：起始、命令、数据长度、校验、结束，共 5 字节
    if (packet->dataLength > MAX_DATA_SIZE || length != (5 + packet->dataLength)) {
        return false;
    }

    // 复制数据
    memcpy(packet->data, &buffer[3], packet->dataLength);
    packet->checksum = buffer[3 + packet->dataLength];
    packet->endByte = buffer[4 + packet->dataLength];

    // 验证校验和
    return Protocol_ValidatePacket(packet);
}

// 验证数据包
bool Protocol_ValidatePacket(CommandPacket *packet) {
    if (!packet) {
        return false;
    }

    // 计算校验和
    // 校验和覆盖命令、数据长度、数据三个字段
    uint8_t calculatedChecksum = Protocol_CalculateChecksum(
        &packet->command,
        2 + packet->dataLength
    );

    // 验证校验和
    return (calculatedChecksum == packet->checksum);
}

// 计算校验和
uint8_t Protocol_CalculateChecksum(const uint8_t *data, uint8_t length) {
    if (!data) {
        return 0;
    }

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }

    return checksum;
}

// 处理命令
Response Protocol_ProcessCommand(CommandPacket *packet) {
    Response response = {
        .command = (CommandTypeEnum) packet->command,
        .success = true,
        .dataLength = 0
    };

    switch (packet->command) {
        case CMD_GET_SENSOR_DATA:
            processGetSensorData(&response);
            break;
        case CMD_GET_ACTUATOR_STATE:
            processGetActuatorState(&response);
            break;
        case CMD_SET_ACTUATOR:
            processSetActuator(&response, packet);
            break;
        case CMD_SET_PARAMS:
            processSetParams(&response, packet);
            break;
        case CMD_GET_PARAMS:
            processGetParams(&response);
            break;
        case CMD_RESET:
            processReset(&response);
            break;
        case CMD_CALIBRATE:
            processCalibrate(&response, packet);
            break;
        case CMD_GET_SYSTEM_INFO:
            processGetSystemInfo(&response);
            break;
        default:
            response.success = false;
            response.dataLength = 0;
            break;
    }

    return response;
}

// 处理获取传感器数据命令
// 土壤湿度，放大 10 倍（0-1000）
// 温度，放大 10 倍（-500-1500）
// 湿度，放大 10 倍（0-1000）
// 光照强度
// 位标志，bit0=土壤有效，bit1=温度有效，bit2=湿度有效，bit3=光敏有效
// 时间戳
static void processGetSensorData(Response *response) {
    AllSensorData fullData;
    if (osMessageQueueGet(Queue_SensorDataHandle, &fullData, NULL, 10) == osOK) {
        CompactSensorData compact;
        // 转换并缩放
        compact.soilMoisture = (uint16_t) (fullData.soilMoisture.value * 10);
        compact.temperature = (int16_t) (fullData.temperature.value * 10);
        compact.humidity = (uint16_t) (fullData.humidity.value * 10);
        compact.lightIntensity = (uint16_t) fullData.lightIntensity.value;
        compact.statusFlags = 0;
        if (fullData.soilMoisture.status == SENSOR_OK) compact.statusFlags |= 0x01;
        if (fullData.temperature.status == SENSOR_OK) compact.statusFlags |= 0x02;
        if (fullData.humidity.status == SENSOR_OK) compact.statusFlags |= 0x04;
        if (fullData.lightIntensity.status == SENSOR_OK) compact.statusFlags |= 0x08;
        compact.timestamp = fullData.lastUpdateTime;

        memcpy(response->data, &compact, sizeof(compact));
        response->dataLength = sizeof(compact);
        response->success = true;
    } else {
        response->success = false;
    }
}

// 处理获取执行器状态命令
// 水泵（0 OFF 1 ON 2 ERROR）
// 风扇（0 OFF 1 ON 2 ERROR）
// 补光灯（0 OFF 1 ON 2 ERROR）
static void processGetActuatorState(Response *response) {
    // 获取执行器状态
    for (int i = 0; i < ACTUATOR_COUNT; i++) {
        ActuatorStateEnum state = ActuatorManager_GetState((ActuatorEnum) i);
        response->data[i] = (uint8_t) state;
    }
    response->dataLength = ACTUATOR_COUNT;
}

// 处理设置执行器命令
static void processSetActuator(Response *response, const CommandPacket *packet) {
    if (packet->dataLength < 2) {
        response->success = false;
        return;
    }

    ActuatorEnum id = (ActuatorEnum) packet->data[0];
    ActuatorStateEnum state = (ActuatorStateEnum) packet->data[1];

    response->success = ActuatorManager_SetState(id, state);
}

// TODO: 处理设置参数命令
static void processSetParams(Response *response, const CommandPacket *packet) {
    if (packet->dataLength < sizeof(ControlParams)) {
        response->success = false;
        return;
    }

    ControlParams params;
    memcpy(&params, packet->data, sizeof(ControlParams));

    response->success = ControllerCore_SetParams(&params);
}

// TODO: 处理获取参数命令
static void processGetParams(Response *response) {
    ControlParams params = ControllerCore_GetParams();
    memcpy(response->data, &params, sizeof(ControlParams));
    response->dataLength = sizeof(ControlParams);
}

// TODO: 处理复位命令
static void processReset(Response *response) {
    // 重置系统
    ControllerCore_ResetToDefaults();
    response->success = true;
}

// TODO: 处理校准命令
static void processCalibrate(Response *response, CommandPacket *packet) {
    // 处理校准命令
    // 这里需要根据校准类型执行不同的校准操作
    response->success = true;
}

// TODO: 处理获取系统信息命令
static void processGetSystemInfo(Response *response) {
    // 填充系统信息
    // 这里需要填充系统版本、运行时间等信息
    response->dataLength = 0;
    response->success = true;
}
