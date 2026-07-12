#include "Utils/shared.h"
#include <stdint.h>
#include <string.h>
#include <Tasks/TelemetryTask.h>

__attribute__((section(".dma_buffer")))
uint8_t TELEMETRY_RX_BUFFER[TELEMETRY_RX_BUFFER_SIZE];

TaskHandle_t TelemetryTaskHandle;

void CreateTelemetryTask(UART_HandleTypeDef *huart, const UBaseType_t Priority, const uint16_t StackSize) {
    xTaskCreate(
        TelemetryTask,
        "TELEMETRY_TASK",
        StackSize,
        huart,
        Priority,
        &TelemetryTaskHandle
    );
}

void TelemetryTask(void *pvParameters) {
    UART_HandleTypeDef *huart = pvParameters;

    HAL_UARTEx_ReceiveToIdle_DMA(huart, TELEMETRY_RX_BUFFER, TELEMETRY_RX_BUFFER_SIZE);

    for (;;) {
        uint32_t Size = 0;
        xTaskNotifyWait(0, UINT32_MAX, &Size, portMAX_DELAY);

        // ParseIncomingPacket(TELEMETRY_RX_BUFFER, (uint16_t)Size);

        HAL_UARTEx_ReceiveToIdle_DMA(huart, TELEMETRY_RX_BUFFER, TELEMETRY_RX_BUFFER_SIZE);
    }
}
