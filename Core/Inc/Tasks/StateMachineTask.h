#ifndef STATEMACHINETASK_H
#define STATEMACHINETASK_H

#include "FreeRTOS.h"
#include "Utils/shared.h"
#include "task.h"

extern TaskHandle_t StateMachineTaskHandle;

void CreateStateMachineTask(SystemContext_t *xSystemContext, UBaseType_t Priority, uint16_t StackSize);
void StateMachineTask(void *pvParameters);

#endif //STATEMACHINETASK_H
