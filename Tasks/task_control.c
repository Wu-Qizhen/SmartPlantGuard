/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "controller_core.h"
#include "bluetooth_bt24.h"
#include "storage_flash.h"
#include "system_status.h"

void StartTask_Control(void *argument) {
    // 初始化
    SystemStatus_Init();
    StorageFlash_Init();
    ActuatorManager_Init();
    ControllerCore_Init();

    // 加载配置，如果失败则使用默认值
    ControlParams params;
    if (StorageFlash_LoadConfig(&params)) {
        // Flash 中有有效配置
        ControllerCore_SetParams(&params);
    }

    for (;;) {
        ControllerCore_RunCycle();
        Bluetooth_UpdateState();
        osDelay(AUTO_CONTROL_INTERVAL_MS);
    }
}
