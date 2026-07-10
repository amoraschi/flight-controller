#ifndef VTELEMETRYTASK_H
#define VTELEMETRYTASK_H

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#define TELEMETRY_RX_BUFFER_SIZE 128

extern uint8_t TELEMETRY_RX_BUFFER[TELEMETRY_RX_BUFFER_SIZE];
extern TaskHandle_t TelemetryTaskHandle;

void vCreateTelemetryTask(UART_HandleTypeDef *huart, UBaseType_t Priority, uint16_t StackSize);
void vTelemetryTask(void *pvParameters);

#endif //VTELEMETRYTASK_H
