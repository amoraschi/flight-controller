#include "States/StateHandlers.h"

void PassiveBurnoutStateEntry(SystemContext_t *ctx) {
}

SystemState_t PassiveBurnoutStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
    return STATE_ACTIVE_BURNOUT;
}
