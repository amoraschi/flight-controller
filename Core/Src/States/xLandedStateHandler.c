#include "States/xLandedStateHandler.h"
#include "stm32h7xx_hal.h"

void vLandedStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t xLandedStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {
    return STATE_LANDED;
}
