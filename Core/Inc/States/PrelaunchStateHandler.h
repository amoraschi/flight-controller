#ifndef XPRELAUNCHSTATEHANDLER_H
#define XPRELAUNCHSTATEHANDLER_H

#include "Utils/shared.h"

void vPrelaunchStateEntry(SystemContext_t *ctx);

SystemState_t xPrelaunchStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status);

#endif //XPRELAUNCHSTATEHANDLER_H
