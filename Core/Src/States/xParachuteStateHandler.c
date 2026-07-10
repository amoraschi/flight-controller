#include "States/xParachuteStateHandler.h"

#include <stdlib.h>

#include "stm32h7xx_hal.h"

void vParachuteStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t xParachuteStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {
    if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_SENSOR_DATA) {
        if ((float)abs((int)StateEvent->SensorData.VelocityZ) < PARACHUTE_LANDED_VEL_Z_THRESHOLD) {
            return STATE_LANDED;
        }
    }

    return STATE_PARACHUTE;
}
