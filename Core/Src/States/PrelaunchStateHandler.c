#include "States/StateHandlers.h"
#include "stm32h7xx_hal.h"

void PrelaunchStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
    ctx->SDLoggingEnabled = true;
}

SystemState_t PrelaunchStateHandler(SystemContext_t *ctx, FlightData_t FlightData, BaseType_t rx_status) {
    if (rx_status == pdPASS) {
        if (FlightData.VelocityZ > PRELAUNCH_BURN_VEL_Z_THRESHOLD) {
            return STATE_BURN;
        }
    }

	// TODO: Refine
    if (SystemFaultFlags != 0) {
        return STATE_GROUND_ABORT;
    }

    return STATE_PRELAUNCH;
}
