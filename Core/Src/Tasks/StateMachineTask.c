#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <States/ApogeeStateHandler.h>
#include <States/BurnStateHandler.h>
#include <States/CalibrationStateHandler.h>
#include <States/IdleStateHandler.h>
#include <States/LandedStateHandler.h>
#include <States/ParachuteStateHandler.h>
#include <States/PrelaunchStateHandler.h>
#include <Tasks/StateMachineTask.h>

#include "Utils/HIL.h"
#include "queue.h"

TaskHandle_t StateMachineTaskHandle;

volatile SystemState_t dbg_current_state = STATE_IDLE;
volatile uint32_t dbg_system_faults = {0};

void vCreateStateMachineTask(SystemContext_t *xSystemContext, const UBaseType_t Priority, const uint16_t StackSize) {
    xTaskCreate(
        vStateMachineTask,
        "STATE_MACHINE_TASK",
        StackSize,
        xSystemContext,
        Priority,
        &StateMachineTaskHandle
    );
}

void vOnStateEntry(const SystemState_t state, SystemContext_t *ctx) {
    switch (state) {
        case STATE_IDLE:
            vIdleStateEntry(ctx);
            break;
        case STATE_CALIBRATION:
            vCalibrationStateEntry(ctx);
            break;
        case STATE_PRELAUNCH:
            vPrelaunchStateEntry(ctx);
            break;
        case STATE_BURN:
            vBurnStateEntry(ctx);
            break;
        case STATE_APOGEE:
            vApogeeStateEntry(ctx);
            break;
        case STATE_PARACHUTE:
            vParachuteStateEntry(ctx);
            break;
        case STATE_LANDED:
            vLandedStateEntry(ctx);
            break;
        default:
            break;
    }
}

SystemState_t xHandleGlobalCommand(SystemState_t CurrenSystemState, StateEvent_t *Event, BaseType_t Received) {
    if (Received != pdPASS || Event->Type != STATE_EVENT_COMMAND) {
        return CurrenSystemState;
    }

    switch (Event->CommandType) {
        case COMMAND_RESET:
            return STATE_IDLE;
        case COMMAND_ABORT:
            return STATE_GROUND_ABORT;
        case COMMAND_CALIBRATION:
            if (CurrenSystemState == STATE_IDLE) {
                return STATE_CALIBRATION;
            }
            return CurrenSystemState;
        default:
            return CurrenSystemState;
    }
}

void vStateMachineTask(void *pvParameters) {
    SystemContext_t *xSystemContext = pvParameters;

    // TODO: Refine
    StateEvent_t LatestStateEvent;
    SystemState_t CurrentSystemState = STATE_IDLE;
    // TODO: Refine

    vOnStateEntry(CurrentSystemState, xSystemContext);

    for (;;) {
        // TODO: Revise timeout
        BaseType_t Received = xQueueReceive(xStateEventQueue, &LatestStateEvent, pdMS_TO_TICKS(100));
        SystemState_t NextSystemState = xHandleGlobalCommand(CurrentSystemState, &LatestStateEvent, Received);

        if (NextSystemState == CurrentSystemState) {
            switch (CurrentSystemState) {
                case STATE_IDLE:
                    NextSystemState = xIdleStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_CALIBRATION:
                    NextSystemState = xCalibrationStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_PRELAUNCH:
                    NextSystemState = xPrelaunchStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_BURN:
                    NextSystemState = xBurnStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_APOGEE:
                    NextSystemState = xApogeeStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_PARACHUTE:
                    NextSystemState = xParachuteStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_LANDED:
                    NextSystemState = xLandedStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                    // TODO: Refine
                case STATE_GROUND_ABORT:
                    NextSystemState = STATE_GROUND_ABORT;
                    break;
                case STATE_ASCENT_ABORT:
                    NextSystemState = STATE_ASCENT_ABORT;
                    break;
                case STATE_DESCENT_ABORT:
                    NextSystemState = STATE_DESCENT_ABORT;
                    break;
                default:
                    // Should not be able to reach
                    NextSystemState = STATE_IDLE;
                    break;
            }
        }

        if (NextSystemState != CurrentSystemState) {
            vOnStateEntry(NextSystemState, xSystemContext);
            CurrentSystemState = NextSystemState;

            // TODO: Remove debug code
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
            vTaskDelay(pdMS_TO_TICKS(10));
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
        }

        if (xSDLoggingQueue != NULL) {
            SDLoggingEvent_t SDLoggingEvent;
            SDLoggingEvent.LatestStateEvent = LatestStateEvent;
            SDLoggingEvent.CurrentSystemState = CurrentSystemState;

            xQueueSend(xSDLoggingQueue, &SDLoggingEvent, 0);
        }

        dbg_current_state = CurrentSystemState;
        dbg_system_faults = xSystemFaultFlags.Flags;

        const uint8_t *pBytes = (const uint8_t *)&LatestStateEvent;
        for (uint32_t i = 0; i < sizeof(StateEvent_t); i++) {
            printf("%02X", pBytes[i]);
        }
        printf("\r\n");
    }
}
