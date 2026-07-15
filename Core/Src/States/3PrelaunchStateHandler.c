#include "States/StateHandlers.h"
#include "stm32h7xx_hal.h"

void PrelaunchStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
    ctx->SDLoggingEnabled = true;
}

SystemState_t PrelaunchStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
	if (FlightData.AccelY > PRELAUNCH_BURN_ACCEL_Y_THRESHOLD) {
		return STATE_BURN;
	}

	// TODO: Refine
    if (SystemFaultFlags != 0) {
        return STATE_GROUND_ABORT;
    }

    return STATE_PRELAUNCH;
}
