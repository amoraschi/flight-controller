#ifndef XAPOGEESTATEHANDLER_H
#define XAPOGEESTATEHANDLER_H

#include "Utils/shared.h"

void vApogeeStateEntry(SystemContext_t *ctx);

SystemState_t xApogeeStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status);

#endif //XAPOGEESTATEHANDLER_H
