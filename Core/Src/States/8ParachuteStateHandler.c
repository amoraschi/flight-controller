#include <stdlib.h>
#include "States/StateHandlers.h"
#include "Utils/Pyro.h"

#include "stm32h7xx_hal.h"

void ParachuteStateEntry(SystemContext_t *ctx) {
    PyroFire(PYRO_CHANNEL_PARACHUTE);
}

SystemState_t ParachuteStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
	if ((float)abs((int)FlightData.VelY) < PARACHUTE_LANDED_VEL_Y_THRESHOLD) {
		return STATE_LANDED;
	}

    return STATE_PARACHUTE;
}
