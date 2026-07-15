#include "Utils/Diagnostics.h"
#include "stm32h7xx_hal.h"
#include "queue.h"

static uint32_t SensorStartCycles[DIAG_SENSOR_COUNT];
static volatile uint32_t SensorReadUs[DIAG_SENSOR_COUNT];
static volatile uint16_t SensorErrorCount[DIAG_SENSOR_COUNT];

static uint32_t SDWriteStartCycles;
static volatile uint32_t SDWriteUs;

static volatile uint32_t LoopUs;
static volatile uint32_t LoopMaxUs;
static volatile uint32_t SDDroppedCount;
static volatile uint8_t LastCommandType;
static volatile uint32_t LastCommandTick;

void DiagnosticsInitDWT(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t DiagnosticsGetCycles(void) {
    return DWT->CYCCNT;
}

uint32_t DiagnosticsCyclesToUs(uint32_t Cycles) {
    return Cycles / (SystemCoreClock / 1000000);
}

void DiagnosticsSensorStart(DiagSensor_t Sensor) {
    SensorStartCycles[Sensor] = DWT->CYCCNT;
}

void DiagnosticsSensorEnd(DiagSensor_t Sensor) {
    uint32_t Delta = DWT->CYCCNT - SensorStartCycles[Sensor];
    SensorReadUs[Sensor] = DiagnosticsCyclesToUs(Delta);
}

void DiagnosticsSDWriteStart(void) {
    SDWriteStartCycles = DWT->CYCCNT;
}

void DiagnosticsSDWriteEnd(void) {
    uint32_t Delta = DWT->CYCCNT - SDWriteStartCycles;
    SDWriteUs = DiagnosticsCyclesToUs(Delta);
}

uint32_t DiagnosticsGetSensorReadUs(DiagSensor_t Sensor) {
    return SensorReadUs[Sensor];
}

uint32_t DiagnosticsGetSDWriteUs(void) {
    return SDWriteUs;
}

void DiagnosticsUpdateLoop(uint32_t StartCycles) {
    uint32_t Us = DiagnosticsCyclesToUs(DWT->CYCCNT - StartCycles);
    LoopUs = Us;
    if (Us > LoopMaxUs) {
        LoopMaxUs = Us;
    }
}

uint32_t DiagnosticsGetLoopUs(void) {
    return LoopUs;
}

uint32_t DiagnosticsGetLoopMaxUs(void) {
    return LoopMaxUs;
}

uint8_t DiagnosticsGetStackUsagePercent(TaskHandle_t Task, uint16_t StackSize, uint32_t Inv) {
    if (Task == NULL) return 0;
    uint16_t Free = (uint16_t)uxTaskGetStackHighWaterMark(Task);
    return (uint8_t)(((StackSize - Free) * Inv) >> STACK_PCT_SHIFT);
}

uint32_t DiagnosticsGetHeapFreeBytes(void) {
    return xPortGetFreeHeapSize();
}

uint32_t DiagnosticsGetHeapMinFreeBytes(void) {
    return xPortGetMinimumEverFreeHeapSize();
}

void DiagnosticsSensorError(DiagSensor_t Sensor) {
    if (SensorErrorCount[Sensor] < UINT16_MAX) {
        SensorErrorCount[Sensor]++;
    }
}

uint16_t DiagnosticsGetSensorErrorCount(DiagSensor_t Sensor) {
    return SensorErrorCount[Sensor];
}

void DiagnosticsSDDropped(void) {
    SDDroppedCount++;
}

uint32_t DiagnosticsGetSDDroppedCount(void) {
    return SDDroppedCount;
}

void DiagnosticsCommandReceived(uint8_t CommandType) {
    LastCommandType = CommandType;
    LastCommandTick = xTaskGetTickCount();
}

uint8_t DiagnosticsGetLastCommandType(void) {
    return LastCommandType;
}

uint32_t DiagnosticsGetLastCommandTick(void) {
    return LastCommandTick;
}
