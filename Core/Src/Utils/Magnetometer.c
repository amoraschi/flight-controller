#include "Utils/Magnetometer.h"

float CalculateMagneticField(uint8_t MSB, uint8_t LSB) {
    int16_t Raw = (int16_t)((MSB << 8) | LSB);
    return (float)Raw * 1.5f;
}
