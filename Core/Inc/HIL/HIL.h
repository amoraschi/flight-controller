#ifndef HIL_H
#define HIL_H

#include <stdint.h>
#include "Utils/configuration.h"

#define HIL_PAYLOAD_SIZE 44

typedef enum {
    HIL_ACCEL_X  = 0,
    HIL_ACCEL_Y  = 4,
    HIL_ACCEL_Z  = 8,
    HIL_GYRO_X   = 12,
    HIL_GYRO_Y   = 16,
    HIL_GYRO_Z   = 20,
    HIL_MAG_X    = 24,
    HIL_MAG_Y    = 28,
    HIL_MAG_Z    = 32,
    HIL_PRESSURE = 36,
    HIL_TEMPERATURE = 40,
} HILPayloadOffset_t;

#if HIL_MODE
void HandleHILPacket(const uint8_t *Payload);
#endif

#endif //HIL_H
