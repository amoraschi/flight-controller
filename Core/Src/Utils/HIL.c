#include "Utils/HIL.h"
#include "Utils/configuration.h"
#include "Utils/shared.h"
#include <string.h>

static ParserState_t CurrentState = PARSER_STATE_IDLE;
static uint8_t PayloadBuffer[sizeof(SensorPayload_t)];
static uint16_t PayloadIndex = 0;
static MsgType_t CurrentMsgType = 0;
static uint16_t ExpectedPayloadSize = 0;

static char *SystemStates[] = {
    "IDLE",
    "CALIBRATION",
    "PRELAUNCH",
    "BURN",
    "APOGEE",
    "PARACHUTE",
    "LANDED"
};

static char *StateEventTypes[] = {
    "SENSOR_DATA",
    "COMMAND"
};

StateEvent_t CreateDataStateEvent(SensorData_t *Data) {
    StateEvent_t Event = {0};
    Event.Type = STATE_EVENT_SENSOR_DATA;
    Event.CommandType = 0;
    Event.SensorData.Sync = Data->Sync;
    Event.SensorData.TimestampMS = Data->TimestampMS;
    Event.SensorData.AccelX = Data->AccelX;
    Event.SensorData.AccelY = Data->AccelY;
    Event.SensorData.AccelZ = Data->AccelZ;
    Event.SensorData.GyroX = Data->GyroX;
    Event.SensorData.GyroY = Data->GyroY;
    Event.SensorData.GyroZ = Data->GyroZ;
    Event.SensorData.MagX = Data->MagX;
    Event.SensorData.MagY = Data->MagY;
    Event.SensorData.MagZ = Data->MagZ;
    Event.SensorData.PressurePa = Data->PressurePa;
    Event.SensorData.TemperatureC = Data->TemperatureC;
    Event.SensorData.Latitude = Data->Latitude;
    Event.SensorData.Longitude = Data->Longitude;
    Event.SensorData.Altitude = Data->Altitude;
    Event.SensorData.VelocityZ = Data->VelocityZ;
    return Event;
}

StateEvent_t CreateCommandStateEvent(CommandType_t CommandType) {
    StateEvent_t Event = {0};
    Event.Type = STATE_EVENT_COMMAND;
    Event.SensorData = (SensorData_t){0};
    Event.CommandType = CommandType;
    return Event;
}

char* GetStateName(const SystemState_t SystemState) {
    switch (SystemState) {
        case STATE_IDLE:
            return SystemStates[0];
        case STATE_CALIBRATION:
            return SystemStates[1];
        case STATE_PRELAUNCH:
            return SystemStates[2];
        case STATE_BURN:
            return SystemStates[3];
        case STATE_APOGEE:
            return SystemStates[4];
        case STATE_PARACHUTE:
            return SystemStates[5];
        case STATE_LANDED:
            return SystemStates[6];
        default:
            return "UNKNOWN";
    }
}

char* GetEventType(const StateEventType_t EventType) {
    switch (EventType) {
        case STATE_EVENT_SENSOR_DATA:
            return StateEventTypes[0];
        case STATE_EVENT_COMMAND:
            return StateEventTypes[1];
        default:
            return "UNKNOWN";
    }
}

static void ParserReset(void) {
    CurrentState = PARSER_STATE_IDLE;
    PayloadIndex = 0;
    CurrentMsgType = 0;
    ExpectedPayloadSize = 0;
}

static bool QueueSensorData(void) {
    SensorPayload_t Data;
    memcpy(&Data, PayloadBuffer, sizeof(Data));

#if HIL_MODE
    if (xHILModeQueue != NULL) {
        xQueueSend(xHILModeQueue, &Data, 0);
    }
#endif

    return true;
}

static bool QueueCommand(void) {
    const CommandType_t CommandType = (CommandType_t)PayloadBuffer[0];

    if (
        CommandType != COMMAND_RESET &&
        CommandType != COMMAND_ABORT &&
        CommandType != COMMAND_CALIBRATION
    ) {
        return false;
    }

#if HIL_MODE || (!HIL_MODE && EXTERNAL_COMMANDS)
    StateEvent_t StateEvent = CreateCommandStateEvent(CommandType);
    if (xStateEventQueue != NULL) {
        xQueueSendToFront(xStateEventQueue, &StateEvent, 0);
    }
#endif

    return true;
}

static bool StartPayload(MsgType_t MsgType) {
    CurrentMsgType = MsgType;
    PayloadIndex = 0;

    switch (MsgType) {
        case MSG_DATA:
            ExpectedPayloadSize = sizeof(SensorPayload_t);
            CurrentState = PARSER_STATE_PAYLOAD;
            break;
        case MSG_COMMAND:
            ExpectedPayloadSize = sizeof(((CommandPacket_t *)0)->command_type);
            CurrentState = PARSER_STATE_PAYLOAD;
            break;
        default:
            ParserReset();
            break;
    }

    return false;
}

static bool FinishPayload(void) {
    switch (CurrentMsgType) {
        case MSG_DATA:
            return QueueSensorData();
        case MSG_COMMAND:
            return QueueCommand();
        default:
            return false;
    }
}

static bool ProcessByte(uint8_t Byte) {
    switch (CurrentState) {
        case PARSER_STATE_IDLE:
            if (Byte == PACKET_HEADER_LSB) {
                CurrentState = PARSER_STATE_HEADER_1;
            }
            return false;
        case PARSER_STATE_HEADER_1:
            if (Byte == PACKET_HEADER_MSB) {
                CurrentState = PARSER_STATE_HEADER_2;
            } else {
                CurrentState = Byte == PACKET_HEADER_LSB ? PARSER_STATE_HEADER_1 : PARSER_STATE_IDLE;
            }
            return false;
        case PARSER_STATE_HEADER_2:
            return StartPayload((MsgType_t)Byte);
        case PARSER_STATE_PAYLOAD:
            PayloadBuffer[PayloadIndex++] = Byte;
            if (PayloadIndex < ExpectedPayloadSize) {
                return false;
            }

            const bool PacketProcessed = FinishPayload();
            ParserReset();
            return PacketProcessed;
    }

    ParserReset();
    return false;
}

bool ParseIncomingPacket(uint8_t *Buffer, const uint16_t Size) {
    if (Buffer == NULL) {
        return false;
    }

    bool PacketFound = false;
    for (uint16_t i = 0; i < Size; i++) {
        if (ProcessByte(Buffer[i])) {
            PacketFound = true;
        }
    }

    return PacketFound;
}
