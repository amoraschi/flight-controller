#ifndef VSDLOGGINGTASK_H
#define VSDLOGGINGTASK_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "Utils/shared.h"
#include "task.h"

extern TaskHandle_t SDLoggingTaskHandle;
extern TaskHandle_t SDWriterTaskHandle;

void CreateSDLoggingTask(SystemContext_t *SystemContext, UBaseType_t Priority, uint16_t StackSize);
void SDLoggingTask(void *pvParameters);
void SDWriterTask(void *pvParameters);

#endif //VSDLOGGINGTASK_H
