#ifndef VSENSORTASK_H
#define VSENSORTASK_H

#include "FreeRTOS.h"
#include "Utils/shared.h"
#include "task.h"

extern TaskHandle_t SensorTaskHandle;

typedef struct {
    SystemHandle_t *Handles;
    SystemContext_t *xSystemContext;
} SensorTaskContext_t;

void vCreateSensorTask(SensorTaskContext_t *TaskContext, UBaseType_t Priority, uint16_t StackSize);
void vSensorTask(void *pvParameters);

#endif //VSENSORTASK_H
