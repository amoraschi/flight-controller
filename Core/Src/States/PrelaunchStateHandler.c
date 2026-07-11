#include "States/StateHandlers.h"
#include "stm32h7xx_hal.h"

void PrelaunchStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
    ctx->SDLoggingEnabled = true;
}

SystemState_t PrelaunchStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {
    if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_SENSOR_DATA) {
        if (StateEvent->SensorData.VelocityZ > PRELAUNCH_BURN_VEL_Z_THRESHOLD) {
            return STATE_BURN;
        }
    }

	if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_COMMAND) {
		if (StateEvent->CommandType == COMMAND_ABORT) {
			return STATE_GROUND_ABORT;
		}
	}

	// TODO: Refine
    if (xSystemFaultFlags.Flags != 0) {
        return STATE_GROUND_ABORT;
    }

    return STATE_PRELAUNCH;
}
