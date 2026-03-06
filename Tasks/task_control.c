/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "actuator_manager.h"
#include "cmsis_os2.h"
#include "controller_core.h"
#include "controller_types.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
// 本地命令类型
typedef enum {
    CMD_SET_MODE = 0,
    CMD_UPDATE_PARAMS,
    CMD_FORCE_ACTION
} LocalCmdType;

// 消息结构体
typedef struct {
    LocalCmdType type;

    union {
        ControlModeEnum mode;
        ControlParams params;

        struct {
            ActuatorEnum act;
            ActuatorStateEnum state;
            uint16_t pwm;
        } action;
    } data;
} LocalMsg;

/* Private variables ---------------------------------------------------------*/
static osMessageQueueId_t controlQueueHandle;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief 系统初始化
 */
static void System_Init(void) {
    // 1. 创建消息队列 (深度 5)
    controlQueueHandle = osMessageQueueNew(5U, sizeof(LocalMsg), NULL);

    // 2. 初始化控制器核心 (内部会 Init 执行器、加载默认参数等)
    if (!ControllerCore_Init()) {
        // 初始化失败处理 (可点亮 LED 报警)
        while (1);
    }
}

/**
 * @brief 处理接收到的命令
 * 直接调用 Core 层提供的接口
 */
static void Process_Command(LocalMsg *msg) {
    if (msg == NULL) return;

    switch (msg->type) {
        case CMD_SET_MODE:
            // 切换模式 (自动/手动/校准)
            ControllerCore_SetMode(msg->data.mode);
            break;

        case CMD_UPDATE_PARAMS:
            // 更新阈值参数
            ControllerCore_SetParams(&msg->data.params);
            // 可选：这里可以加一行代码保存到 Flash
            // StorageFlash_SaveParams(&msg->data.params);
            break;

        case CMD_FORCE_ACTION:
            // 强制动作 (手动模式下使用)
            if (msg->data.action.pwm > 0) {
                ControllerCore_ManualPWM(msg->data.action.act, msg->data.action.pwm);
            } else {
                ControllerCore_ManualControl(msg->data.action.act, msg->data.action.state);
            }
            break;

        default:
            break;
    }
}

void StartTask_Control(void *argument) {
    LocalMsg msg;

    // 1. 初始化
    System_Init();

    for (;;) {
        // 尝试接收消息，等待 50ms
        // 如果有消息，立即处理；如果没有，超时后继续往下走
        if (osMessageQueueGet(controlQueueHandle, &msg, NULL, 50U) == osOK) {
            Process_Command(&msg);

            // 收到命令后，立即运行一次控制循环以快速响应
            ControllerCore_RunCycle();
        } else {
            // 没有消息，执行正常的自动控制循环
            // 这一步会：读传感器 -> 判断阈值 -> 开关水泵/风扇
            ControllerCore_RunCycle();
        }

        // 3. 剩余延时，保持约 100ms 的控制周期
        // 50ms (等待消息) + ~10-20ms (运行逻辑) + 30-40ms (延时) = 100ms
        osDelay(30U);
    }
}

/**
 * @brief 外部接口：发送命令到控制任务
 * 供串口、LCD 等其他模块调用
 *
 * @param type 命令类型
 * @param mode 模式 (CMD_SET_MODE 时使用)
 * @param p 参数指针 (CMD_UPDATE_PARAMS 时使用)
 * @param act 执行器 (CMD_FORCE_ACTION 时使用)
 * @param st 状态 (CMD_FORCE_ACTION 时使用)
 * @param pwm PWM 占空比 (CMD_FORCE_ACTION 时使用)
 */
osStatus_t TaskControl_SendCmd(LocalCmdType type, ControlModeEnum mode, ControlParams *p, ActuatorEnum act,
                               ActuatorStateEnum st, uint16_t pwm) {
    if (controlQueueHandle == NULL) return osErrorResource;

    LocalMsg m;
    m.type = type;
    memset(&m.data, 0, sizeof(m.data));

    if (type == CMD_SET_MODE) {
        m.data.mode = mode;
    } else if (type == CMD_UPDATE_PARAMS && p != NULL) {
        m.data.params = *p;
    } else if (type == CMD_FORCE_ACTION) {
        m.data.action.act = act;
        m.data.action.state = st;
        m.data.action.pwm = pwm;
    }

    return osMessageQueuePut(controlQueueHandle, &m, 0U, osWaitForever);
}
