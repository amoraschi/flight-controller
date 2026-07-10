#ifndef XCALIBRATIONSTATEHANDLER_H
#define XCALIBRATIONSTATEHANDLER_H

#include "Utils/shared.h"

void vCalibrationStateEntry(SystemContext_t *ctx);

SystemState_t xCalibrationStateHandler(SystemContext_t *ctx, StateEvent_t *StateEvent, BaseType_t rx_status);

#endif //XCALIBRATIONSTATEHANDLER_H
