#include "Utils/IMU.h"
#include "Utils/configuration.h"

float CalculateGyroscope(uint8_t MSB, uint8_t LSB, float Factor) {
    return ((int16_t)((MSB << 8) | LSB)) * Factor;
}

float CalculateAcceleration(uint8_t MSB, uint8_t LSB, float Factor) {
    return ((int16_t)((MSB << 8) | LSB)) * Factor;
}

void CalibrateGyroscope(FlightData_t FlightData, SystemContext_t *SystemContext, float *GyroSumX, float *GyroSumY, float *GyroSumZ, uint16_t *GyroSampleCount, uint16_t *GyroDiscardCount) {
    if (SystemContext->GyroCalibrationValid) {
        return;
    }

    if ((*GyroDiscardCount) < GYRO_CALIBRATION_DISCARD_SAMPLES) {
        (*GyroDiscardCount)++;
        return;
    }

    (*GyroSumX) += FlightData.GyroX;
    (*GyroSumY) += FlightData.GyroY;
    (*GyroSumZ) += FlightData.GyroZ;
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
