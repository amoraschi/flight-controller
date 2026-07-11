#include "States/StateHandlers.h"
#include "stm32h7xx_hal.h"

void LandedStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t LandedStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {
    return STATE_LANDED;
}
