#include <stdlib.h>
#include "States/StateHandlers.h"

#include "stm32h7xx_hal.h"

void ParachuteStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t ParachuteStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
	if ((float)abs((int)FlightData.VelY) < PARACHUTE_LANDED_VEL_Z_THRESHOLD) {
		return STATE_LANDED;
	}

    return STATE_PARACHUTE;
}
