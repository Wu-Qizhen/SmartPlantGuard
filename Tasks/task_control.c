/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "system_status.h"
#include "actuator_manager.h"
#include "bluetooth_bt24.h"
#include "controller_core.h"

void StartTask_Control(void *argument) {
    // 初始化
    SystemStatus_Init();
    ActuatorManager_Init();
    ControllerCore_Init();

    for (;;) {
        ControllerCore_RunCycle();
        Bluetooth_UpdateState();
        osDelay(AUTO_CONTROL_INTERVAL_MS);
    }
}
