#ifndef ALTITUDE_H
#define ALTITUDE_H

#include "shared.h"

#define GAS_CONSTANT    287.0f // J/(kg*K)
#define GRAV_CONSTANT   9.80665f // m/s^2

float CalculateAltitude(float PressurePa, float Temperature, float ReferencePressurePa, bool ReferencePressurePaValid);
float CalculateFilteredAltitude(SystemContext_t *SystemContext, float RawAltitude);

#endif //ALTITUDE_H
