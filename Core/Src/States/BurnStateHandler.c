#include <States/BurnStateHandler.h>
#include "stm32h7xx_hal.h"

void vBurnStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t xBurnStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {
    if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_SENSOR_DATA) {
        // TODO: Revise, hysteresis, use acceleration if possible
        if (StateEvent->SensorData.VelocityZ < BURN_APOGEE_VEL_Z_THRESHOLD) {
            return STATE_APOGEE;
        }
    }

    return STATE_BURN;
}
