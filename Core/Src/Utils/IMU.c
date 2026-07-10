#include "Utils/IMU.h"
#include "Utils/configuration.h"

float CalculateGyroscope(uint8_t MSB, uint8_t LSB, float Factor) {
    return ((int16_t)((MSB << 8) | LSB)) * Factor;
}

float CalculateAcceleration(uint8_t MSB, uint8_t LSB, float Factor) {
    return ((int16_t)((MSB << 8) | LSB)) * Factor;
}

void CalibrateAccelerometer(StateEvent_t *StateEvent, SystemContext_t *SystemContext, float *AccelSumX, float *AccelSumY, float *AccelSumZ, uint16_t *AccelSampleCount, uint16_t *AccelDiscardCount) {
    if (SystemContext->AccelCalibrationValid) {
        return;
    }

    if ((*AccelDiscardCount) < ACCEL_CALIBRATION_DISCARD_SAMPLES) {
        (*AccelDiscardCount)++;
        return;
    }

    (*AccelSumX) += StateEvent->SensorData.AccelX;
    (*AccelSumY) += StateEvent->SensorData.AccelY;
    (*AccelSumZ) += StateEvent->SensorData.AccelZ;
    (*AccelSampleCount)++;

    if ((*AccelSampleCount) >= ACCEL_CALIBRATION_SAMPLES) {
        const float InvCount = 1.0f / (float)(*AccelSampleCount);
        SystemContext->AccelBiasX = (*AccelSumX) * InvCount;
        SystemContext->AccelBiasY = (*AccelSumY) * InvCount;
        SystemContext->AccelBiasZ = (*AccelSumZ) * InvCount;
        SystemContext->AccelCalibrationValid = true;
    }
}

float CalculateBiasedAcceleration(SystemContext_t *SystemContext, float Value, float Bias) {
    return SystemContext->AccelCalibrationValid ? Value - Bias : Value;
}

void CalibrateGyroscope(StateEvent_t *StateEvent, SystemContext_t *SystemContext, float *GyroSumX, float *GyroSumY, float *GyroSumZ, uint16_t *GyroSampleCount, uint16_t *GyroDiscardCount) {
    if (SystemContext->GyroCalibrationValid) {
        return;
    }

    if ((*GyroDiscardCount) < GYRO_CALIBRATION_DISCARD_SAMPLES) {
        (*GyroDiscardCount)++;
        return;
    }

    (*GyroSumX) += StateEvent->SensorData.GyroX;
    (*GyroSumY) += StateEvent->SensorData.GyroY;
    (*GyroSumZ) += StateEvent->SensorData.GyroZ;
    (*GyroSampleCount)++;

    if ((*GyroSampleCount) >= GYRO_CALIBRATION_SAMPLES) {
        const float InvCount = 1.0f / (float)(*GyroSampleCount);
        SystemContext->GyroBiasX = (*GyroSumX) * InvCount;
        SystemContext->GyroBiasY = (*GyroSumY) * InvCount;
        SystemContext->GyroBiasZ = (*GyroSumZ) * InvCount;
        SystemContext->GyroCalibrationValid = true;
    }
}

float CalculateBiasedGyroscope(SystemContext_t *SystemContext, float Value, float Bias) {
    return SystemContext->GyroCalibrationValid ? Value - Bias : Value;
}
