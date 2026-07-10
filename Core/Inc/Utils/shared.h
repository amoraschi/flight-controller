#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "configuration.h"
#include "stm32h7xx_hal.h"

#define BIT(n) (1U << (n))
#define BITS_SET(value, mask) (((value) & (mask)) == (mask))
#define BITS_CLEAR(value, mask) (((value) & (mask)) == 0)

typedef enum {
    MSG_DATA = 0x01,
    MSG_COMMAND = 0x02
} MsgType_t;

typedef enum {
    COMMAND_RESET = 0x01,
    COMMAND_ABORT = 0x02,
    COMMAND_CALIBRATION = 0x03
} CommandType_t;

typedef enum {
    SENSOR_MODE_OFF,
    SENSOR_MODE_IDLE,
    SENSOR_MODE_PERFORMANCE
} SensorMode_t;

typedef enum {
    STATE_IDLE,         // Initialize sensors at low power
    STATE_CALIBRATION,  // Remote command, maximize sensor output for calibration
    STATE_PRELAUNCH,    // Initialize SD logging
    STATE_BURN,         // Acceleration detected
    STATE_APOGEE,       // Apogee detected, drogue parachute deploy
    STATE_PARACHUTE,    // Altitude threshold, main parachute deploy
    STATE_LANDED,       // Landed, turn off except GPS

    STATE_GROUND_ABORT,
    STATE_ASCENT_ABORT,
    STATE_DESCENT_ABORT,

    STATE_MAX           // Table size
} SystemState_t;

// TODO: Refine
typedef struct {
    float ReferencePressurePa;
    bool ReferencePressurePaValid;
    float AccelBiasX;
    float AccelBiasY;
    float AccelBiasZ;
    bool AccelCalibrationValid;
    float GyroBiasX;
    float GyroBiasY;
    float GyroBiasZ;
    bool GyroCalibrationValid;
    volatile bool AltitudeFilterInitialized;
    volatile bool SDLoggingEnabled;
    volatile bool SensorsIdleFinished;
} SystemContext_t;

// TODO: Refine
typedef struct {
    I2C_HandleTypeDef *BMP280_Handle;
    I2C_HandleTypeDef *BMP581_Handle;
    SPI_HandleTypeDef *IIM42653_Handle;
    I2C_HandleTypeDef *IIS2MDCTR_Handle;
    TIM_HandleTypeDef *TIM2_Handle;
} SystemHandle_t;

typedef enum {
    BMP280_MODE_IDLE_FAILED = 1u << 0,
    BMP280_MODE_PERFORMANCE_FAILED = 1u << 1,
    BMP581_MODE_IDLE_FAILED = 1u << 2,
    BMP581_MODE_PERFORMANCE_FAILED = 1u << 3,
    IIM42653_MODE_IDLE_FAILED = 1u << 4,
    IIM42653_MODE_PERFORMANCE_FAILED = 1u << 5,
    IIS2MDCTR_MODE_IDLE_FAILED = 1u << 6,
    IIS2MDCTR_MODE_PERFORMANCE_FAILED = 1u << 7,
    SD_MOUNT_FAILED = 1u << 8,
    SD_OPEN_FAILED = 1u << 9,
    // TODO: Refine
} SystemFaultFlag_t;

typedef struct {
    uint32_t Flags;
} SystemFaultFlags_t;

#pragma pack(push, 1)
typedef struct {
    uint32_t timestamp_ms;
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
    int16_t mx;
    int16_t my;
    int16_t mz;
    float press;
    float temp;
    int32_t lat;
    int32_t lon;
} SensorPayload_t;

typedef struct {
    uint16_t header;
    uint8_t  msg_type;
    SensorPayload_t payload;
} SensorPacket_t;

typedef struct {
    uint16_t header;
    uint8_t  msg_type;
    uint8_t  command_type;
} CommandPacket_t;

typedef struct {
    uint16_t Sync;
    uint32_t TimestampMS;
    float AccelX;
    float AccelY;
    float AccelZ;
    float GyroX;
    float GyroY;
    float GyroZ;
    float MagX;
    float MagY;
    float MagZ;
    float PressurePa;
    float TemperatureC;
    int32_t Latitude;
    int32_t Longitude;
    float Altitude;
    float VelocityZ;
} SensorData_t;
#pragma pack(pop)

typedef enum {
    STATE_EVENT_SENSOR_DATA,
    STATE_EVENT_COMMAND
} StateEventType_t;

#pragma pack(push, 1)
typedef struct {
    StateEventType_t Type;
    SensorData_t SensorData;
    CommandType_t CommandType;
} StateEvent_t;

typedef struct {
    StateEvent_t LatestStateEvent;
    SystemState_t CurrentSystemState;
} SDLoggingEvent_t;
#pragma pack(pop)

extern QueueHandle_t xStateEventQueue;
extern QueueHandle_t xSensorModeQueue;
extern QueueHandle_t xSDLoggingQueue;

#if HIL_MODE
extern QueueHandle_t xHILModeQueue;
#endif

extern SystemFaultFlags_t xSystemFaultFlags;
#endif //SHARED_H
