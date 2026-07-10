#ifndef VSTATEMACHINETASK_H
#define VSTATEMACHINETASK_H

#include "FreeRTOS.h"
#include "Utils/shared.h"
#include "task.h"

extern TaskHandle_t StateMachineTaskHandle;

void vCreateStateMachineTask(SystemContext_t *xSystemContext, UBaseType_t Priority, uint16_t StackSize);
void vStateMachineTask(void *pvParameters);

#endif //VSTATEMACHINETASK_H
