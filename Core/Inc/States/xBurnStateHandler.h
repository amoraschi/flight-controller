#ifndef XBURNSTATEHANDLER_H
#define XBURNSTATEHANDLER_H

#include "Utils/shared.h"

void vBurnStateEntry(SystemContext_t *ctx);

SystemState_t xBurnStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status);

#endif //XBURNSTATEHANDLER_H
