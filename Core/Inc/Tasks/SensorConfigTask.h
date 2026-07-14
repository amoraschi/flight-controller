#ifndef SENSORCONFIGTASK_H
#define SENSORCONFIGTASK_H

#include "FreeRTOS.h"
#include "Utils/shared.h"
#include "task.h"

extern TaskHandle_t SensorConfigTaskHandle;

void CreateSensorConfigTask(SystemContext_t *SystemContext, UBaseType_t Priority, uint16_t StackSize);
void SensorConfigTask(void *pvParameters);

#endif //SENSORCONFIGTASK_H
