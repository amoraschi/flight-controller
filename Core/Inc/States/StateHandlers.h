#ifndef STATEHANDLERS_H
#define STATEHANDLERS_H

#include "Utils/shared.h"

void IdleStateEntry(SystemContext_t *Context);
void CalibrationStateEntry(SystemContext_t *Context);
void PrelaunchStateEntry(SystemContext_t *Context);
void BurnStateEntry(SystemContext_t *Context);
void ApogeeStateEntry(SystemContext_t *Context);
void ParachuteStateEntry(SystemContext_t *Context);
void LandedStateEntry(SystemContext_t *Context);

SystemState_t IdleStateHandler(SystemContext_t *Context, FlightData_t FlightData, BaseType_t RxStatus);
SystemState_t CalibrationStateHandler(SystemContext_t *Context, FlightData_t FlightData, BaseType_t RxStatus);
SystemState_t PrelaunchStateHandler(SystemContext_t *Context, FlightData_t FlightData, BaseType_t RxStatus);
SystemState_t BurnStateHandler(SystemContext_t *Context, FlightData_t FlightData, BaseType_t RxStatus);
SystemState_t ApogeeStateHandler(SystemContext_t *Context, FlightData_t FlightData, BaseType_t RxStatus);
SystemState_t ParachuteStateHandler(SystemContext_t *Context, FlightData_t FlightData, BaseType_t RxStatus);
SystemState_t LandedStateHandler(SystemContext_t *Context, FlightData_t FlightData, BaseType_t RxStatus);

void OnStateEntry(const SystemState_t CurrentSystemState, SystemContext_t *SystemContext);
void HandleSensors(SystemContext_t *SystemContext, SystemState_t CurrentSystemState);
SystemState_t HandleCommand(SystemState_t CurrentSystemState, CommandType_t CommandType, BaseType_t Received);
SystemState_t HandleState(SystemState_t CurrentSystemState, SystemContext_t *SystemContext, FlightData_t FlightData, BaseType_t Received);

#endif // STATEHANDLERS_H
