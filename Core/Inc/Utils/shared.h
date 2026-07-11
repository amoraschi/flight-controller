#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "configuration.h"
#include "stm32h7xx_hal.h"
#include "timers.h"

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

typedef uint32_t SystemFaultFlags_t;

#pragma pack(push, 1)
typedef struct {
    uint16_t Sync;
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
    uint32_t Flags;
    uint8_t State;
} FlightData_t;
#pragma pack(pop)

extern QueueHandle_t SDLoggingQueue;

#if HIL_MODE
extern QueueHandle_t HILModeQueue;
#endif

extern TimerHandle_t TimerIIM42653;
extern TimerHandle_t TimerBMP581;
extern TimerHandle_t TimerIIS2MDCTR;

extern SystemFaultFlags_t SystemFaultFlags;

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern SD_HandleTypeDef hsd1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

#define BMP581_HANDLE      (&hi2c2)
#define IIM42653_HANDLE    (&hspi2)
#define IIS2MDCTR_HANDLE   (&hi2c1)
#define SD_HANDLE          (&hsd1)
#define TIM2_HANDLE        (&htim2)
#define USART1_HANDLE      (&huart1)

#endif //SHARED_H
