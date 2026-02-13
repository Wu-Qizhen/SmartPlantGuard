/**
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.13
 */
#include "cmsis_os2.h"
#include "main.h"
#include "sensor_manager.h"
#include "sensor_types.h"

void StartTask_SensorRead(void *argument) {
    AllSensorData sensorData;

    SensorManager_Init();

    for (;;) {
        if (SensorManager_ReadAll(&sensorData)) {
            osMessageQueuePut(Queue_SensorDataHandle, &sensorData, 0, 100);
        }

        osDelay(2000);
    }
}
