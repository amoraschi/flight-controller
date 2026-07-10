#ifndef XIDLESTATEHANDLER_H
#define XIDLESTATEHANDLER_H

#include "Utils/shared.h"

void vIdleStateEntry(SystemContext_t *ctx);

SystemState_t xIdleStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status);

#endif //XIDLESTATEHANDLER_H
