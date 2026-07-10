#ifndef VSDLOGGINGTASK_H
#define VSDLOGGINGTASK_H

#include "FreeRTOS.h"
#include "Utils/shared.h"
#include "task.h"

extern TaskHandle_t SDLoggingTaskHandle;

void vCreateSDLoggingTask(SystemContext_t *xSystemContext, UBaseType_t Priority, uint16_t StackSize);
void vSDLoggingTask(void *pvParameters);

#endif //VSDLOGGINGTASK_H
