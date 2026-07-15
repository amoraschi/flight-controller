#include "States/StateHandlers.h"
#include "Utils/Pyro.h"
#include "stm32h7xx_hal.h"

void LandedStateEntry(SystemContext_t *ctx) {
    PyroSafeAll();
}

SystemState_t LandedStateHandler(SystemContext_t *Context, FlightData_t FlightData) {
    return STATE_LANDED;
}
