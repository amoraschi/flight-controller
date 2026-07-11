#include "Utils/configuration.h"
#include "Utils/Barometer.h"

#include <stdbool.h>

const float PRESSURE_SCALE = 1.0f / 64.0f;       // BMP581: 6 fractional bits
const float TEMPERATURE_SCALE = 1.0f / 65536.0f; // BMP581: 16 fractional bits

uint32_t CalculateUncompensatedPressure(const uint8_t *DataBuffer) {
    return (((uint32_t)DataBuffer[0]) << 12) |
           (((uint32_t)DataBuffer[1]) << 4) |
           (((uint32_t)DataBuffer[2]) >> 4);
}

uint32_t CalculateUncompensatedTemperature(const uint8_t *DataBuffer) {
    return (((uint32_t)DataBuffer[3]) << 12) |
           (((uint32_t)DataBuffer[4]) << 4) |
           (((uint32_t)DataBuffer[5]) >> 4);
}

// TODO: Remove
volatile float dbg_ref_presscount;
volatile float dbg_ref_presssum;
volatile float dbg_ref_press;
volatile float dbg_ref_discard;
volatile float dbg_curr_press;

void CalibratePressure(FlightData_t FlightData, SystemContext_t *SystemContext, float *PressureSumPa, uint16_t *PressureSampleCount, uint16_t *PressureDiscardCount) {
    float PressurePa = FlightData.PressurePa;
    dbg_curr_press = PressurePa;

    if (PressurePa > 0.0f && !SystemContext->ReferencePressurePaValid) {
        if ((*PressureDiscardCount) < PRESSURE_CALIBRATION_DISCARD_SAMPLES) {
            dbg_ref_discard++;
            (*PressureDiscardCount)++;
            return;
        }

        (*PressureSumPa) += PressurePa;
        (*PressureSampleCount)++;

        dbg_ref_presssum += PressurePa;
        dbg_ref_presscount++;

        if ((*PressureSampleCount) >= PRESSURE_CALIBRATION_SAMPLES) {
            dbg_ref_press = (*PressureSumPa) / (float)(*PressureSampleCount);
            SystemContext->ReferencePressurePa = (*PressureSumPa) / (float)(*PressureSampleCount);
            SystemContext->ReferencePressurePaValid = true;
        }
    }
}

float CalculatePressureTemperature(uint8_t MSB, uint8_t LSB, uint8_t XLSB, bool Temperature) {
    int32_t RawValue = (int32_t)((MSB << 16) | (LSB << 8) | XLSB);
    int32_t SignValue = (RawValue << 8) >> 8;

    return (float)SignValue * (Temperature ? TEMPERATURE_SCALE : PRESSURE_SCALE);
}
