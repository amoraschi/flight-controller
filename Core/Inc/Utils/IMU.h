#ifndef IMU_H
#define IMU_H

#include <stdint.h>

#include "shared.h"

float CalculateGyroscope(uint8_t MSB, uint8_t LSB, float Factor);
float CalculateAcceleration(uint8_t MSB, uint8_t LSB, float Factor);

void CalibrateGyroscope(FlightData_t FlightData, SystemContext_t *SystemContext, float *GyroSumX, float *GyroSumY, float *GyroSumZ, uint16_t *GyroSampleCount, uint16_t *GyroDiscardCount);
float CalculateBiasedGyroscope(SystemContext_t *SystemContext, float Value, float Bias);

#endif //IMU_H
