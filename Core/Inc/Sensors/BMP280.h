#ifndef BMP280_H
#define BMP280_H

#include "stm32h7xx_hal.h"

#define BMP280_ID       0x58
#define BMP280_RESET    0xB6

#define BMP280_REG_RESET        0xE0
#define BMP280_REG_ID           0xD0
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_CONFIG       0xF5
#define BMP280_REG_CALIB00      0x88
#define BMP280_REG_PRESS_MSB    0xF7

#define BMP280_CALIBRATION_DATA_SIZE    24
#define BMP280_SENSOR_DATA_SIZE         6

/* I2C Address (SDO tied to GND) */
#define BMP280_I2C_ADDRESS        (0x76 << 1)

#define BMP280_TEMP_OVERSAMPLING_X1     (0x01 << 5)
#define BMP280_TEMP_OVERSAMPLING_X2     (0x02 << 5)
#define BMP280_TEMP_OVERSAMPLING_X4     (0x03 << 5)
#define BMP280_TEMP_OVERSAMPLING_X8     (0x04 << 5)
#define BMP280_TEMP_OVERSAMPLING_X16    (0x05 << 5)

#define BMP280_PRESS_OVERSAMPLING_X1    (0x01 << 2)
#define BMP280_PRESS_OVERSAMPLING_X2    (0x02 << 2)
#define BMP280_PRESS_OVERSAMPLING_X4    (0x03 << 2)
#define BMP280_PRESS_OVERSAMPLING_X8    (0x04 << 2)
#define BMP280_PRESS_OVERSAMPLING_X16   (0x05 << 2)

#define BMP280_MODE_SLEEP   (0x00 << 0)
#define BMP280_MODE_FORCED  (0x01 << 0)
#define BMP280_MODE_NORMAL  (0x03 << 0)

#define BMP280_STANDBY_MS_0_5   (0x00 << 5)
#define BMP280_STANDBY_MS_62_5  (0x01 << 5)
#define BMP280_STANDBY_MS_125   (0x02 << 5)
#define BMP280_STANDBY_MS_250   (0x03 << 5)
#define BMP280_STANDBY_MS_500   (0x04 << 5)
#define BMP280_STANDBY_MS_1000  (0x05 << 5)
#define BMP280_STANDBY_MS_2000  (0x06 << 5)
#define BMP280_STANDBY_MS_4000  (0x07 << 5)

#define BMP280_FILTER_OFF       (0x00 << 2)
#define BMP280_FILTER_COEFF_2   (0x01 << 2)
#define BMP280_FILTER_COEFF_4   (0x02 << 2)
#define BMP280_FILTER_COEFF_8   (0x03 << 2)
#define BMP280_FILTER_COEFF_16  (0x04 << 2)

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_CalibrationData_t;

typedef struct {
    float temperature_c;
    float pressure_pa;
} BMP280_SensorData_t;

HAL_StatusTypeDef BMP280_Reset(I2C_HandleTypeDef *BMP280_Handle);
HAL_StatusTypeDef BMP280_CheckChipID(I2C_HandleTypeDef *BMP280_Handle);
HAL_StatusTypeDef BMP280_ReadCalibrationData(I2C_HandleTypeDef *BMP280_Handle);
HAL_StatusTypeDef BMP280_SetConfig(I2C_HandleTypeDef *BMP280_Handle, uint8_t Flags);
HAL_StatusTypeDef BMP280_SetCtrlMeas(I2C_HandleTypeDef *BMP280_Handle, uint8_t Flags);
HAL_StatusTypeDef BMP280_DummyRead(I2C_HandleTypeDef *BMP280_Handle);
HAL_StatusTypeDef BMP280_ReadData(I2C_HandleTypeDef *BMP280_Handle, BMP280_SensorData_t *SensorData);

#endif //BMP280_H
