#include <Tasks/SensorConfigTask.h>
#include "Managers/Managers.h"

TaskHandle_t SensorConfigTaskHandle;

void CreateSensorConfigTask(SystemContext_t *SystemContext, const UBaseType_t Priority, const uint16_t StackSize) {
    xTaskCreate(
        SensorConfigTask,
        "SENSOR_CONFIG",
        StackSize,
        SystemContext,
        Priority,
        &SensorConfigTaskHandle
    );
}

void SensorConfigTask(void *pvParameters) {
    SystemContext_t *SystemContext = pvParameters;

    for (;;) {
        uint32_t TargetState = 0;
        xTaskNotifyWait(0, UINT32_MAX, &TargetState, portMAX_DELAY);
        HandleSensors(SystemContext, (SystemState_t)TargetState);
    }
}
