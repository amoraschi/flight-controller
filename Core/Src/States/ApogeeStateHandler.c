#include "States/StateHandlers.h"
#include "stm32h7xx_hal.h"

void ApogeeStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t ApogeeStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
	if (FlightData.Altitude < APOGEE_PARACHUTE_ALTITUDE_THRESHOLD) {
		return STATE_PARACHUTE;
	}

    return STATE_APOGEE;
}
