#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <States/StateHandlers.h>
#include <Tasks/StateMachineTask.h>

#include "Utils/HIL.h"
#include "queue.h"

TaskHandle_t StateMachineTaskHandle;

volatile SystemState_t dbg_current_state = STATE_IDLE;
volatile uint32_t dbg_system_faults = {0};

SystemState_t HandleGlobalCommand(SystemState_t CurrenSystemState, StateEvent_t *Event, BaseType_t Received) {
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

void CreateStateMachineTask(SystemContext_t *xSystemContext, const UBaseType_t Priority, const uint16_t StackSize) {
    xTaskCreate(
        StateMachineTask,
        "STATE_MACHINE_TASK",
        StackSize,
        xSystemContext,
        Priority,
        &StateMachineTaskHandle
    );
}

void OnStateEntry(const SystemState_t State, SystemContext_t *Context) {
    switch (State) {
        case STATE_IDLE:
            IdleStateEntry(Context);
            break;
        case STATE_CALIBRATION:
            CalibrationStateEntry(Context);
            break;
        case STATE_PRELAUNCH:
            PrelaunchStateEntry(Context);
            break;
        case STATE_BURN:
            BurnStateEntry(Context);
            break;
        case STATE_APOGEE:
            ApogeeStateEntry(Context);
            break;
        case STATE_PARACHUTE:
            ParachuteStateEntry(Context);
            break;
        case STATE_LANDED:
            LandedStateEntry(Context);
            break;
        default:
            break;
    }
}

void StateMachineTask(void *pvParameters) {
    SystemContext_t *xSystemContext = pvParameters;

    // TODO: Refine
    StateEvent_t LatestStateEvent;
    SystemState_t CurrentSystemState = STATE_IDLE;

    OnStateEntry(CurrentSystemState, xSystemContext);

    for (;;) {
        // TODO: Revise timeout
        SystemState_t NextSystemState = HandleGlobalCommand(CurrentSystemState, &LatestStateEvent, Received);

        if (NextSystemState == CurrentSystemState) {
            switch (CurrentSystemState) {
                case STATE_IDLE:
                    NextSystemState = IdleStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_CALIBRATION:
                    NextSystemState = CalibrationStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_PRELAUNCH:
                    NextSystemState = PrelaunchStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_BURN:
                    NextSystemState = BurnStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_APOGEE:
                    NextSystemState = ApogeeStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_PARACHUTE:
                    NextSystemState = ParachuteStateHandler(xSystemContext, &LatestStateEvent, Received);
                    break;
                case STATE_LANDED:
                    NextSystemState = LandedStateHandler(xSystemContext, &LatestStateEvent, Received);
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
            OnStateEntry(NextSystemState, xSystemContext);
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
