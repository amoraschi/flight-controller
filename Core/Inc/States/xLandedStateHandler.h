#ifndef XLANDEDSTATEHANDLER_H
#define XLANDEDSTATEHANDLER_H

#include "Utils/shared.h"

void vLandedStateEntry(SystemContext_t *ctx);

SystemState_t xLandedStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status);

#endif //XLANDEDSTATEHANDLER_H
