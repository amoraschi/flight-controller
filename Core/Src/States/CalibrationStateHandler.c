#include <States/CalibrationStateHandler.h>
#include "Utils/Barometer.h"
#include "Utils/IMU.h"
#include "Utils/Velocity.h"

static float PressureSumPa;
static uint16_t PressureSampleCount;
static uint16_t PressureDiscardCount;

static float AccelSumX, AccelSumY, AccelSumZ;
static uint16_t AccelSampleCount;
static uint16_t AccelDiscardCount;

static float GyroSumX, GyroSumY, GyroSumZ;
static uint16_t GyroSampleCount;
static uint16_t GyroDiscardCount;

void vCalibrationStateEntry(SystemContext_t *ctx) {
    // TODO: Refine
    ctx->ReferencePressurePa = 0.0f;
    ctx->ReferencePressurePaValid = 0;

    ctx->AccelBiasX = 0.0f;
    ctx->AccelBiasY = 0.0f;
    ctx->AccelBiasZ = 0.0f;
    ctx->AccelCalibrationValid = false;

    ctx->GyroBiasX = 0.0f;
    ctx->GyroBiasY = 0.0f;
    ctx->GyroBiasZ = 0.0f;
    ctx->GyroCalibrationValid = false;

    PressureSumPa = 0.0f;
    PressureSampleCount = 0;
    PressureDiscardCount = 0;

    AccelSumX = 0.0f;
    AccelSumY = 0.0f;
    AccelSumZ = 0.0f;
    AccelSampleCount = 0;
    AccelDiscardCount = 0;

    GyroSumX = 0.0f;
    GyroSumY = 0.0f;
    GyroSumZ = 0.0f;
    GyroSampleCount = 0;
    GyroDiscardCount = 0;

    ResetVelocityZWindow();

    const SensorMode_t mode = SENSOR_MODE_PERFORMANCE;
    xQueueOverwrite(xSensorModeQueue, &mode);
}

SystemState_t xCalibrationStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status) {
    if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_SENSOR_DATA) {
        CalibratePressure(StateEvent, ctx, &PressureSumPa, &PressureSampleCount, &PressureDiscardCount);
//        CalibrateAccelerometer(StateEvent, ctx, &AccelSumX, &AccelSumY, &AccelSumZ, &AccelSampleCount, &AccelDiscardCount);
        CalibrateGyroscope(StateEvent, ctx, &GyroSumX, &GyroSumY, &GyroSumZ, &GyroSampleCount, &GyroDiscardCount);

        ctx->AccelCalibrationValid = 1;
        if (ctx->ReferencePressurePaValid && ctx->AccelCalibrationValid && ctx->GyroCalibrationValid) {
        	// TODO: Dirty race condition fix
            StateEvent->SensorData.AccelX = CalculateBiasedAcceleration(ctx, StateEvent->SensorData.AccelX, ctx->AccelBiasX);
            StateEvent->SensorData.AccelY = CalculateBiasedAcceleration(ctx, StateEvent->SensorData.AccelY, ctx->AccelBiasY);
            StateEvent->SensorData.AccelZ = CalculateBiasedAcceleration(ctx, StateEvent->SensorData.AccelZ, ctx->AccelBiasZ);
            StateEvent->SensorData.GyroX = CalculateBiasedGyroscope(ctx, StateEvent->SensorData.GyroX, ctx->GyroBiasX);
            StateEvent->SensorData.GyroY = CalculateBiasedGyroscope(ctx, StateEvent->SensorData.GyroY, ctx->GyroBiasY);
            StateEvent->SensorData.GyroZ = CalculateBiasedGyroscope(ctx, StateEvent->SensorData.GyroZ, ctx->GyroBiasZ);

            return STATE_PRELAUNCH;
        }
    }

    if (rx_status == pdPASS && StateEvent->Type == STATE_EVENT_COMMAND) {
        if (StateEvent->CommandType == COMMAND_ABORT) {
            return STATE_GROUND_ABORT;
        }
    }

    // TODO: Refine
    if (xSystemFaultFlags.Flags != 0) {
        return STATE_GROUND_ABORT;
    }

    // Remain on Calibration if not valid command
    return STATE_CALIBRATION;
}
