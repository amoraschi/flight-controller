#ifndef XPARACHUTESTATEHANDLER_H
#define XPARACHUTESTATEHANDLER_H

#include "Utils/shared.h"

void vParachuteStateEntry(SystemContext_t *ctx);

SystemState_t xParachuteStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status);

#endif //XPARACHUTESTATEHANDLER_H
