#include "States/xIdleStateHandler.h"
#include "stm32h7xx_hal.h"

void vIdleStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
    ctx->ReferencePressurePa = 0.0f;
    ctx->ReferencePressurePaValid = 0;

    // TODO: Refine
    const SensorMode_t mode = SENSOR_MODE_IDLE;
    xQueueOverwrite(xSensorModeQueue, &mode);
}

SystemState_t xIdleStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {;
    if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_COMMAND) {
        if (StateEvent->CommandType == COMMAND_CALIBRATION) {
            return STATE_CALIBRATION;
        }
    }

    // TODO: URGENT: Test code, remove later
    if (ctx->SensorsIdleFinished) {
    	return STATE_CALIBRATION;
    }

    // TODO: Refine
    if (xSystemFaultFlags.Flags != 0) {
        return STATE_GROUND_ABORT;
    }

    // Remain on Idle if not valid command
    return STATE_IDLE;
}
