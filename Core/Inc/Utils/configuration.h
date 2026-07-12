#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define STATE_EVENT_QUEUE_LENGTH    3
#define SENSOR_MODE_QUEUE_LENGTH    1

#define PACKET_HEADER      0xCAFE
#define PACKET_HEADER_LSB  (PACKET_HEADER & 0xFF)
#define PACKET_HEADER_MSB  ((PACKET_HEADER >> 8) & 0xFF)

#define HIL_MODE                    0
#define EXTERNAL_COMMANDS           1
#define AUTO_START_CALIBRATION		1
// TODO: Update
// #define SCALE_HEIGHT                5000
// #define PRESSURE_0                  101325.0f

// Altitude configuration
#define ALTITUDE_IIR_FILTER_ALPHA    0.1f

// Transition Configuration
#define PRELAUNCH_BURN_VEL_Z_THRESHOLD      20.0f
#define BURN_APOGEE_VEL_Z_THRESHOLD         5.0f
#define APOGEE_PARACHUTE_ALTITUDE_THRESHOLD 500.0f
#define PARACHUTE_LANDED_VEL_Z_THRESHOLD    5.0f

// Barometer Configuration
#define PRESSURE_CALIBRATION_DISCARD_SAMPLES    200
#define PRESSURE_CALIBRATION_SAMPLES            500

// Accelerometer Configuration
#define ACCEL_CALIBRATION_DISCARD_SAMPLES       200
#define ACCEL_CALIBRATION_SAMPLES               500

// Gyroscope Configuration
#define GYRO_CALIBRATION_DISCARD_SAMPLES        200
#define GYRO_CALIBRATION_SAMPLES                500

// SD Configuration
#define SD_LOGGING_RECORDS_PER_BUFFER       500
#define SD_SYNC_INTERVAL_TICKS 				pdMS_TO_TICKS(1000)
#define SD_LOG_SYNC                         0xCAFE
#define SD_LOG_SYNC_END                     0xBE

#endif //CONFIGURATION_H
