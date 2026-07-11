#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <Sensors/Sensors.h>
#include <States/StateHandlers.h>
#include <Tasks/StateMachineTask.h>
#include <Utils/FlightData.h>
#include "queue.h"

TaskHandle_t StateMachineTaskHandle;

volatile SystemState_t dbg_current_state = STATE_IDLE;
volatile uint32_t dbg_system_faults = {0};
volatile FlightData_t dbg_flight_data;

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

void StateMachineTask(void *pvParameters) {
    SystemContext_t *SystemContext = pvParameters;

    // TODO: Refine
    FlightData_t FlightData = {0};
    SystemState_t CurrentSystemState = STATE_IDLE;

    OnStateEntry(CurrentSystemState, SystemContext);
	HandleSensors(SystemContext, CurrentSystemState);

	BUZZER_Beep(100);

    for (;;) {
        // TODO: Revise timeout
    	BaseType_t Received = pdPASS;
        BMP581_SensorData_t BMP581_SensorData;
        IIM42653_SensorData_t IIM42653_SensorData;
        IIS2MDCTR_SensorData_t IIS2MDCTR_SensorData;

        BMP581_Mailbox_Read(&BMP581_SensorData);
        IIM42653_Mailbox_Read(&IIM42653_SensorData);
        IIS2MDCTR_Mailbox_Read(&IIS2MDCTR_SensorData);

        FlightData = GetFlightData(CurrentSystemState, SystemContext, IIM42653_SensorData, BMP581_SensorData, IIS2MDCTR_SensorData);
        dbg_flight_data = FlightData;

        // SystemState_t NextSystemState = HandleCommand(CurrentSystemState, &LatestStateEvent, pdFAIL);
        SystemState_t NextSystemState = CurrentSystemState;

        if (NextSystemState == CurrentSystemState) {
            NextSystemState = HandleState(CurrentSystemState, SystemContext, FlightData, Received);
        }

        if (NextSystemState != CurrentSystemState) {
            OnStateEntry(NextSystemState, SystemContext);
            HandleSensors(SystemContext, NextSystemState);
            CurrentSystemState = NextSystemState;

            // TODO: Remove debug code
             BUZZER_Beep(5);
        }

        if (SDLoggingQueue != NULL) {
            xQueueSend(SDLoggingQueue, &FlightData, 0);
        }

        dbg_current_state = CurrentSystemState;
        dbg_system_faults = SystemFaultFlags;

        const uint8_t *pBytes = (const uint8_t *)&FlightData;
        for (uint32_t i = 0; i < sizeof(FlightData_t); i++) {
            printf("%02X", pBytes[i]);
        }
        printf("\r\n");

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
