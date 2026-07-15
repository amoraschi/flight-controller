#ifndef CALIBRATIONS_H
#define CALIBRATIONS_H

#include <stdint.h>
#include "shared.h"

void CalibrateGyroscope(FlightData_t FlightData, SystemContext_t *SystemContext, float *GyroSumX, float *GyroSumY, float *GyroSumZ, uint16_t *GyroSampleCount, uint16_t *GyroDiscardCount);
void CalibratePressure(FlightData_t FlightData, SystemContext_t *SystemContext, float *PressureSumPa, uint16_t *PressureSampleCount, uint16_t *PressureDiscardCount);

#endif //CALIBRATIONS_H
