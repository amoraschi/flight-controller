#include "Utils/Calibrations.h"

void CalibratePressure(FlightData_t FlightData, SystemContext_t *SystemContext, float *PressureSumPa, uint16_t *PressureSampleCount, uint16_t *PressureDiscardCount) {
    float PressurePa = FlightData.PressurePa;

    if (PressurePa > 0.0f && !SystemContext->ReferencePressurePaValid) {
        if ((*PressureDiscardCount) < PRESSURE_CALIBRATION_DISCARD_SAMPLES) {
            (*PressureDiscardCount)++;
            return;
        }

        (*PressureSumPa) += PressurePa;
        (*PressureSampleCount)++;

        if ((*PressureSampleCount) >= PRESSURE_CALIBRATION_SAMPLES) {
            SystemContext->ReferencePressurePa = (*PressureSumPa) / (float)(*PressureSampleCount);
            SystemContext->ReferencePressurePaValid = true;
        }
    }
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
