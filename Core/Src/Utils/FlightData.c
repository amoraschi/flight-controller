#include "Utils/shared.h"
#include "Sensors/Sensors.h"
#include "Utils/Battery.h"
#include "Utils/Calculations.h"
#include "Utils/Diagnostics.h"
#include <Tasks/StateMachineTask.h>
#include <Tasks/SDLoggingTask.h>
#include <Tasks/TelemetryTask.h>

static uint8_t CachedStackStateMachine;
static uint8_t CachedStackSDProducer;
static uint8_t CachedStackSDWriter;
static uint8_t CachedStackTelemetry;
static uint32_t StackUpdateCounter;

FlightData_t GetFlightData(SystemState_t SystemState, SystemContext_t *SystemContext, IIM42653_SensorData_t IIM42653_FlightData, BMP581_SensorData_t BMP581_FlightData, IIS2MDCTR_SensorData_t IIS2MDCTR_FlightData) {
	FlightData_t FlightData;

	if (++StackUpdateCounter >= 100) {
		StackUpdateCounter = 0;
		CachedStackStateMachine = DiagnosticsGetStackUsagePercent(StateMachineTaskHandle, STACK_SIZE_STATE_MACHINE, STACK_INV_STATE_MACHINE);
		CachedStackSDProducer = DiagnosticsGetStackUsagePercent(SDProducerTaskHandle, STACK_SIZE_SD_LOGGING, STACK_INV_SD_LOGGING);
		CachedStackSDWriter = DiagnosticsGetStackUsagePercent(SDWriterTaskHandle, STACK_SIZE_SD_LOGGING, STACK_INV_SD_LOGGING);
		CachedStackTelemetry = DiagnosticsGetStackUsagePercent(TelemetryTaskHandle, STACK_SIZE_TELEMETRY, STACK_INV_TELEMETRY);
	}

	FlightData.Sync = PACKET_HEADER;
	FlightData.Tick = xTaskGetTickCount();

	FlightData.PressurePa = BMP581_FlightData.PressurePa;
	FlightData.TemperatureC = BMP581_FlightData.TemperatureC;

	FlightData.MagX = IIS2MDCTR_FlightData.MagX;
	FlightData.MagY = IIS2MDCTR_FlightData.MagY;
	FlightData.MagZ = IIS2MDCTR_FlightData.MagZ;

	FlightData.GyroX = CalculateBiasedGyroscope(SystemContext, IIM42653_FlightData.GyroX, SystemContext->GyroBiasX);
	FlightData.GyroY = CalculateBiasedGyroscope(SystemContext, IIM42653_FlightData.GyroY, SystemContext->GyroBiasY);
	FlightData.GyroZ = CalculateBiasedGyroscope(SystemContext, IIM42653_FlightData.GyroZ, SystemContext->GyroBiasZ);

	FlightData.AccelX = IIM42653_FlightData.AccelX;
	FlightData.AccelY = IIM42653_FlightData.AccelY;
	FlightData.AccelZ = IIM42653_FlightData.AccelZ;

	FlightData.Latitude = 0;
	FlightData.Longitude = 0;

	FlightData.RawAltitude = CalculateAltitude(SystemContext, FlightData.PressurePa, FlightData.TemperatureC);
	FlightData.Altitude = CalculateFilteredAltitude(SystemContext, FlightData.RawAltitude);

	FlightData.VelX = 0;
	FlightData.VelY = 0;
	FlightData.VelZ = 0;

	FlightData.VelX = 0;
	FlightData.VelY = 0;
	FlightData.VelZ = 0;

	FlightData.Flags = SystemFaultFlags;
	FlightData.BatteryVoltage = 0; // BatteryGetVoltage();
	FlightData.LoopExecutionUs = DiagnosticsGetLoopUs();
	FlightData.LoopMaxExecutionUs = DiagnosticsGetLoopMaxUs();
	FlightData.BMP581ReadUs = DiagnosticsGetSensorReadUs(DIAG_SENSOR_BMP581);
	FlightData.IIM42653ReadUs = DiagnosticsGetSensorReadUs(DIAG_SENSOR_IIM42653);
	FlightData.IIS2MDCTRReadUs = DiagnosticsGetSensorReadUs(DIAG_SENSOR_IIS2MDCTR);
	FlightData.SDWriteUs = DiagnosticsGetSDWriteUs();
	FlightData.StackUsageStateMachine = CachedStackStateMachine;
	FlightData.StackUsageSDProducer = CachedStackSDProducer;
	FlightData.StackUsageSDWriter = CachedStackSDWriter;
	FlightData.StackUsageTelemetry = CachedStackTelemetry;
	FlightData.HeapFreeBytes = DiagnosticsGetHeapFreeBytes();
	FlightData.HeapMinFreeBytes = DiagnosticsGetHeapMinFreeBytes();
	FlightData.BMP581ErrorCount = DiagnosticsGetSensorErrorCount(DIAG_SENSOR_BMP581);
	FlightData.IIM42653ErrorCount = DiagnosticsGetSensorErrorCount(DIAG_SENSOR_IIM42653);
	FlightData.IIS2MDCTRErrorCount = DiagnosticsGetSensorErrorCount(DIAG_SENSOR_IIS2MDCTR);
	FlightData.SDDroppedCount = DiagnosticsGetSDDroppedCount();
	FlightData.LastCommandType = DiagnosticsGetLastCommandType();
	FlightData.LastCommandTick = DiagnosticsGetLastCommandTick();
	FlightData.State = SystemState;
	FlightData.SyncEnd = PACKET_FOOTER;

	return FlightData;
}
