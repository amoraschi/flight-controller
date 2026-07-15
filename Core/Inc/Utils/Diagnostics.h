#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    DIAG_SENSOR_BMP581,
    DIAG_SENSOR_IIM42653,
    DIAG_SENSOR_IIS2MDCTR,
    DIAG_SENSOR_COUNT
} DiagSensor_t;

void DiagnosticsInitDWT(void);
uint32_t DiagnosticsGetCycles(void);
uint32_t DiagnosticsCyclesToUs(uint32_t Cycles);

void DiagnosticsSensorStart(DiagSensor_t Sensor);
void DiagnosticsSensorEnd(DiagSensor_t Sensor);
void DiagnosticsSDWriteStart(void);
void DiagnosticsSDWriteEnd(void);

uint32_t DiagnosticsGetSensorReadUs(DiagSensor_t Sensor);
uint32_t DiagnosticsGetSDWriteUs(void);
uint32_t DiagnosticsGetLoopUs(void);
uint32_t DiagnosticsGetLoopMaxUs(void);
void DiagnosticsUpdateLoop(uint32_t StartCycles);

void DiagnosticsUpdateSystemHealth(void);
#include "Utils/configuration.h"

#define STACK_PCT_SHIFT             16
#define STACK_INV_STATE_MACHINE     ((100 << STACK_PCT_SHIFT) / STACK_SIZE_STATE_MACHINE)
#define STACK_INV_SD_LOGGING        ((100 << STACK_PCT_SHIFT) / STACK_SIZE_SD_LOGGING)
#define STACK_INV_TELEMETRY         ((100 << STACK_PCT_SHIFT) / STACK_SIZE_TELEMETRY)

uint8_t DiagnosticsGetStackUsagePercent(TaskHandle_t Task, uint16_t StackSize, uint32_t Inv);
uint32_t DiagnosticsGetHeapFreeBytes(void);
uint32_t DiagnosticsGetHeapMinFreeBytes(void);
void DiagnosticsSensorError(DiagSensor_t Sensor);
uint16_t DiagnosticsGetSensorErrorCount(DiagSensor_t Sensor);

void DiagnosticsSDDropped(void);
uint32_t DiagnosticsGetSDDroppedCount(void);

void DiagnosticsCommandReceived(uint8_t CommandType);
uint8_t DiagnosticsGetLastCommandType(void);
uint32_t DiagnosticsGetLastCommandTick(void);

#endif // DIAGNOSTICS_H
