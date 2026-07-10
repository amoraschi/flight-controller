#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdbool.h>

#include "shared.h"

typedef enum {
    PARSER_STATE_IDLE,
    PARSER_STATE_HEADER_1,
    PARSER_STATE_HEADER_2,
    PARSER_STATE_PAYLOAD
} ParserState_t;

StateEvent_t CreateDataStateEvent(SensorData_t *Data);
char* GetStateName(SystemState_t SystemState);
char* GetEventType(StateEventType_t EventType);
bool ParseIncomingPacket(uint8_t *Buffer, uint16_t Size);

#endif //PARSER_H
