#include "States/StateHandlers.h"
#include "stm32h7xx_hal.h"

void BurnStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t BurnStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
	// TODO: Revise, hysteresis, use acceleration if possible
	if (FlightData.VelY < BURN_APOGEE_VEL_Y_THRESHOLD) {
		return STATE_PASSIVE_BURNOUT;
	}

    return STATE_BURN;
}
