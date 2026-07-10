#include "States/xApogeeStateHandler.h"
#include "stm32h7xx_hal.h"

void vApogeeStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
}

SystemState_t xApogeeStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {
    if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_SENSOR_DATA) {
        if (StateEvent->SensorData.Altitude < APOGEE_PARACHUTE_ALTITUDE_THRESHOLD) {
            return STATE_PARACHUTE;
        }
    }

    return STATE_APOGEE;
}
