#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "shared.h"

uint32_t CalculateUncompensatedPressure(const uint8_t *DataBuffer);
uint32_t CalculateUncompensatedTemperature(const uint8_t *DataBuffer);
void CalibratePressure(FlightData_t FlightData, SystemContext_t *SystemContext, float *PressureSumPa, uint16_t *PressureSampleCount, uint16_t *PressureDiscardCount);

float CalculatePressureTemperature(uint8_t MSB, uint8_t LSB, uint8_t XLSB, bool Temperature);

#endif //CALCULATIONS_H
