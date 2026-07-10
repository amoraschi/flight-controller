#ifndef VSDLOGGINGTASK_H
#define VSDLOGGINGTASK_H

#include "FreeRTOS.h"
#include "Utils/shared.h"
#include "task.h"

extern TaskHandle_t SDLoggingTaskHandle;

void CreateSDLoggingTask(SystemContext_t *xSystemContext, UBaseType_t Priority, uint16_t StackSize);
void SDLoggingTask(void *pvParameters);

#endif //VSDLOGGINGTASK_H
