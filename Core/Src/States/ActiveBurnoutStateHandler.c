#include "States/StateHandlers.h"

void ActiveBurnoutStateEntry(SystemContext_t *ctx) {
}

SystemState_t ActiveBurnoutStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
    return STATE_APOGEE;
}
