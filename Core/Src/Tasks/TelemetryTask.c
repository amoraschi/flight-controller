#include "stm32h7xx_hal.h"
#include "Utils/HIL.h"
#include "queue.h"
#include "Utils/shared.h"

#include <stdint.h>
#include <string.h>
#include <Tasks/TelemetryTask.h>

/* TODO: */
__attribute__((section(".dma_buffer"))) uint8_t TELEMETRY_RX_BUFFER[TELEMETRY_RX_BUFFER_SIZE];

TaskHandle_t TelemetryTaskHandle;

void vCreateTelemetryTask(UART_HandleTypeDef *huart, const UBaseType_t Priority, const uint16_t StackSize) {
    xTaskCreate(
        vTelemetryTask,
        "TELEMETRY_TASK",
        StackSize,
        huart,
        Priority,
        &TelemetryTaskHandle
    );
}

void vTelemetryTask(void *pvParameters) {
    UART_HandleTypeDef *huart = pvParameters;

    HAL_UARTEx_ReceiveToIdle_DMA(huart, TELEMETRY_RX_BUFFER, TELEMETRY_RX_BUFFER_SIZE);

    for (;;) {
        uint32_t Size = 0;
        // The notification value carries the received byte count for this DMA event.
        xTaskNotifyWait(0, UINT32_MAX, &Size, portMAX_DELAY);

        ParseIncomingPacket(TELEMETRY_RX_BUFFER, (uint16_t)Size);

        HAL_UARTEx_ReceiveToIdle_DMA(huart, TELEMETRY_RX_BUFFER, TELEMETRY_RX_BUFFER_SIZE);
    }
}
