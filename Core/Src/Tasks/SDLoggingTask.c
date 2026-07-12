#include <Tasks/SDLoggingTask.h>
#include "Utils/SD.h"

TaskHandle_t SDLoggingTaskHandle;

static SDLoggingBuffer_t BufferA;
static SDLoggingBuffer_t BufferB;
static SDLoggingBuffer_t *ActiveBuffer = &BufferA;
static SDLoggingBuffer_t *WriteBuffer = NULL;

volatile uint64_t dbg_write_fault = 0;

void CreateSDLoggingTask(SystemContext_t *SystemContext, const UBaseType_t Priority, const uint16_t StackSize) {
    xTaskCreate(
        SDLoggingTask,
        "SD_LOGGING_TASK",
        StackSize,
        SystemContext,
        Priority,
        &SDLoggingTaskHandle
    );
}

void SDLoggingTask(void *pvParameters) {
    SystemContext_t *SystemContext = pvParameters;

    MountAndOpen();

    for (;;) {
        FlightData_t FlightData;

        HAL_StatusTypeDef SDQueueStatus = xQueueReceive(SDLoggingQueue, &FlightData, 0);

        if (SDQueueStatus == pdPASS && SystemContext->SDLoggingEnabled) {
			ActiveBuffer->Records[ActiveBuffer->Count++] = FlightData;

			if (ActiveBuffer->Count >= SD_LOGGING_RECORDS_PER_BUFFER) {
				WriteBuffer = ActiveBuffer;
				ActiveBuffer = (ActiveBuffer == &BufferA) ? &BufferB : &BufferA;
				ActiveBuffer->Count = 0;

				if (WriteLoggingBuffer(WriteBuffer) != FR_OK) {
					dbg_write_fault++;
				}

				WriteBuffer->Count = 0;
				f_sync(&SDFile);
			}
        }

        // if (!xSystemContext->SDLoggingEnabled && SDLoggingEvent.CurrentSystemState == STATE_LANDED) {
        //     if (ActiveBuffer->Count > 0) {
        //         WriteLoggingBuffer(ActiveBuffer);
        //         ActiveBuffer->Count = 0;
        //     }
        //
        //     f_sync(&SDFile);
        // }
    }
}
