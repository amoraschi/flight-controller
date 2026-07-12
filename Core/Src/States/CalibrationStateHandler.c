#include "States/StateHandlers.h"
#include "Utils/Barometer.h"
#include "Utils/IMU.h"
#include "Utils/Velocity.h"

static float PressureSumPa;
static uint16_t PressureSampleCount;
static uint16_t PressureDiscardCount;

static float GyroSumX, GyroSumY, GyroSumZ;
static uint16_t GyroSampleCount;
static uint16_t GyroDiscardCount;

void CalibrationStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
    ctx->ReferencePressurePa = 0.0f;
    ctx->ReferencePressurePaValid = 0;

    ctx->GyroBiasX = 0.0f;
    ctx->GyroBiasY = 0.0f;
    ctx->GyroBiasZ = 0.0f;
    ctx->GyroCalibrationValid = false;

    PressureSumPa = 0.0f;
    PressureSampleCount = 0;
    PressureDiscardCount = 0;

    GyroSumX = 0.0f;
    GyroSumY = 0.0f;
    GyroSumZ = 0.0f;
    GyroSampleCount = 0;
    GyroDiscardCount = 0;

    ResetVelocityZWindow();
}

SystemState_t CalibrationStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
	CalibratePressure(FlightData, Context, &PressureSumPa, &PressureSampleCount, &PressureDiscardCount);
	CalibrateGyroscope(FlightData, Context, &GyroSumX, &GyroSumY, &GyroSumZ, &GyroSampleCount, &GyroDiscardCount);

	if (Context->ReferencePressurePaValid && Context->GyroCalibrationValid) {
		return STATE_PRELAUNCH;
	}

    // TODO: Refine
    if (SystemFaultFlags != 0) {
        return STATE_GROUND_ABORT;
    }

    // Remain on Calibration if not valid command
    return STATE_CALIBRATION;
}
