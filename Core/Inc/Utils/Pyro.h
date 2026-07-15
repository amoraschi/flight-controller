#ifndef PYRO_H
#define PYRO_H

#include "stm32h7xx_hal.h"

typedef enum {
    PYRO_CHANNEL_DROGUE,
    PYRO_CHANNEL_PARACHUTE,
} PyroChannel_t;

void PyroFire(PyroChannel_t Channel);
void PyroSafe(PyroChannel_t Channel);
void PyroSafeAll(void);

#endif //PYRO_H
