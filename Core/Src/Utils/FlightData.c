#include "Utils/shared.h"
#include "Sensors/Sensors.h"
#include "Utils/IMU.h"
#include "Utils/Altitude.h"
#include "Utils/Battery.h"

FlightData_t GetFlightData(SystemState_t SystemState, SystemContext_t *SystemContext, IIM42653_SensorData_t IIM42653_FlightData, BMP581_SensorData_t BMP581_FlightData, IIS2MDCTR_SensorData_t IIS2MDCTR_FlightData) {
	FlightData_t FlightData;

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

	FlightData.Altitude = CalculateAltitude(SystemContext, FlightData.PressurePa, FlightData.TemperatureC);
	FlightData.Altitude = CalculateFilteredAltitude(SystemContext, FlightData.Altitude);

	FlightData.Flags = SystemFaultFlags;
//	FlightData.BatteryVoltage = BatteryGetVoltage();
	FlightData.State = SystemState;
	FlightData.SyncEnd = PACKET_FOOTER;

	return FlightData;
}
