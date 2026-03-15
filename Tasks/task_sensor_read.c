/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "sensor_manager.h"

void StartTask_SensorRead(void *argument) {
    AllSensorData sensorData;
    // SensorManager_Init();

    for (;;) {
        if (SensorManager_ReadAll(&sensorData)) {
            // 获取互斥量，更新全局变量
            osMutexAcquire(gSensorDataMutex, osWaitForever);
            gLatestSensorData = sensorData; // 结构体赋值（可能较慢，但可接受）
            osMutexRelease(gSensorDataMutex);
        }

        osDelay(SENSOR_READ_INTERVAL_MS);
    }
}
