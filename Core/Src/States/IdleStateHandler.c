#include "States/StateHandlers.h"
#include "stm32h7xx_hal.h"

void IdleStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
    ctx->ReferencePressurePa = 0.0f;
    ctx->ReferencePressurePaValid = 0;
}

SystemState_t IdleStateHandler(SystemContext_t *ctx, FlightData_t FlightData, BaseType_t rx_status) {
    // TODO: URGENT: Test code, remove later
    if (ctx->SensorsIdleFinished) {
    	return STATE_CALIBRATION;
    }

    // TODO: Refine
    if (SystemFaultFlags != 0) {
        return STATE_GROUND_ABORT;
    }

    // Remain on Idle if not valid command
    return STATE_IDLE;
}
