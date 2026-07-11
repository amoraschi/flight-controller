#include <stdlib.h>
#include "States/StateHandlers.h"

#include "stm32h7xx_hal.h"

void ParachuteStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t ParachuteStateHandler(SystemContext_t *ctx, FlightData_t FlightData, BaseType_t rx_status) {
    if (rx_status == pdPASS) {
        if ((float)abs((int)FlightData.VelocityZ) < PARACHUTE_LANDED_VEL_Z_THRESHOLD) {
            return STATE_LANDED;
        }
    }

    return STATE_PARACHUTE;
}
