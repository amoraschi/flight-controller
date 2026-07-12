#ifndef MANAGERS_H
#define MANAGERS_H

void SerialInit(void);
void SerialSendFlightData(const FlightData_t *FlightData);

void OnStateEntry(const SystemState_t CurrentSystemState, SystemContext_t *SystemContext);
void HandleSensors(SystemContext_t *SystemContext, SystemState_t CurrentSystemState);
SystemState_t HandleCommand(SystemState_t CurrentSystemState, CommandType_t CommandType, BaseType_t Received);
SystemState_t HandleState(SystemState_t CurrentSystemState, SystemContext_t *SystemContext, FlightData_t FlightData, BaseType_t Received);

#endif // MANAGERS_H
