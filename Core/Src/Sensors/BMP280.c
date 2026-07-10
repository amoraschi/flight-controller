#include "Sensors/BMP280.h"

#include "FreeRTOS.h"
#include "Utils/Barometer.h"

static BMP280_CalibrationData_t bmp280_calibration_data;
// Fine temperature resolution for pressure calculation
static int32_t t_fine;
static uint8_t data_buffer[BMP280_SENSOR_DATA_SIZE] = {0};

static HAL_StatusTypeDef BMP280_ReadRegister(I2C_HandleTypeDef *BMP280_Handle, uint8_t reg, uint8_t *data, uint16_t size) {
    if (BMP280_Handle == NULL || data == NULL) {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Read(BMP280_Handle, BMP280_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, size, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

static HAL_StatusTypeDef BMP280_WriteRegister(I2C_HandleTypeDef *BMP280_Handle, uint8_t reg, uint8_t *data, uint16_t size) {
    if (BMP280_Handle == NULL || data == NULL) {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Write(BMP280_Handle, BMP280_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, size, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

// Returns temperature in DegC, resolution is 0.01 DegC.
// Output value of "5123" equals 51.23 DegC.
static int32_t BMP280_CompensateTemperature(int32_t uncomp_temperature) {
    int32_t var1 = ((((uncomp_temperature >> 3) - ((int32_t)bmp280_calibration_data.dig_T1 << 1))) * ((int32_t)bmp280_calibration_data.dig_T2)) >> 11;
    int32_t var2 = (((((uncomp_temperature >> 4) - ((int32_t)bmp280_calibration_data.dig_T1)) * ((uncomp_temperature >> 4) - ((int32_t)bmp280_calibration_data.dig_T1))) >> 12) * ((int32_t)bmp280_calibration_data.dig_T3)) >> 14;

    t_fine = var1 + var2;
    int32_t T = (t_fine * 5 + 128) >> 8;

    return T;
}

// Returns pressure in Pa as unsigned 32-bit integer in Q24.8 format
// (24 integer bits and 8 fractional bits).
// Output value of "24674867" represents 24674867 / 256 = 96386.2 Pa = 963.862 hPa
static uint32_t BMP280_CompensatePressure(int32_t uncomp_pressure) {
    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)bmp280_calibration_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)bmp280_calibration_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)bmp280_calibration_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bmp280_calibration_data.dig_P3) >> 8) + ((var1 * (int64_t)bmp280_calibration_data.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)bmp280_calibration_data.dig_P1) >> 33;

    if (var1 == 0) {
        return 0; // Avoid exception caused by division by zero
    }

    int64_t p = 1048576 - uncomp_pressure;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bmp280_calibration_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bmp280_calibration_data.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280_calibration_data.dig_P7) << 4);

    return (uint32_t)p;
}

HAL_StatusTypeDef BMP280_Reset(I2C_HandleTypeDef *BMP280_Handle) {
    uint8_t reset_cmd = BMP280_RESET;
    return BMP280_WriteRegister(BMP280_Handle, BMP280_REG_RESET, &reset_cmd, 1);
}

HAL_StatusTypeDef BMP280_CheckChipID(I2C_HandleTypeDef *BMP280_Handle) {
    uint8_t ChipID = 0;

    if (BMP280_ReadRegister(BMP280_Handle, BMP280_REG_ID, &ChipID, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (ChipID != BMP280_ID) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef BMP280_ReadCalibrationData(I2C_HandleTypeDef *BMP280_Handle) {
    uint8_t trim_buffer[BMP280_CALIBRATION_DATA_SIZE] = {0};

    if (BMP280_ReadRegister(BMP280_Handle, BMP280_REG_CALIB00, trim_buffer, BMP280_CALIBRATION_DATA_SIZE) != HAL_OK) {
        return HAL_ERROR;
    }

    bmp280_calibration_data.dig_T1 = (uint16_t)((trim_buffer[1] << 8) | trim_buffer[0]);
    bmp280_calibration_data.dig_T2 = (int16_t) ((trim_buffer[3] << 8) | trim_buffer[2]);
    bmp280_calibration_data.dig_T3 = (int16_t) ((trim_buffer[5] << 8) | trim_buffer[4]);

    bmp280_calibration_data.dig_P1 = (uint16_t)((trim_buffer[7] << 8) | trim_buffer[6]);
    bmp280_calibration_data.dig_P2 = (int16_t) ((trim_buffer[9] << 8) | trim_buffer[8]);
    bmp280_calibration_data.dig_P3 = (int16_t) ((trim_buffer[11] << 8) | trim_buffer[10]);
    bmp280_calibration_data.dig_P4 = (int16_t) ((trim_buffer[13] << 8) | trim_buffer[12]);
    bmp280_calibration_data.dig_P5 = (int16_t) ((trim_buffer[15] << 8) | trim_buffer[14]);
    bmp280_calibration_data.dig_P6 = (int16_t) ((trim_buffer[17] << 8) | trim_buffer[16]);
    bmp280_calibration_data.dig_P7 = (int16_t) ((trim_buffer[19] << 8) | trim_buffer[18]);
    bmp280_calibration_data.dig_P8 = (int16_t) ((trim_buffer[21] << 8) | trim_buffer[20]);
    bmp280_calibration_data.dig_P9 = (int16_t) ((trim_buffer[23] << 8) | trim_buffer[22]);

    return HAL_OK;
}

HAL_StatusTypeDef BMP280_SetConfig(I2C_HandleTypeDef *BMP280_Handle, uint8_t Flags) {
    uint8_t Value = Flags;
    return BMP280_WriteRegister(BMP280_Handle, BMP280_REG_CONFIG, &Value, 1);
}

HAL_StatusTypeDef BMP280_SetCtrlMeas(I2C_HandleTypeDef *BMP280_Handle, uint8_t Flags) {
    uint8_t Value = Flags;
    return BMP280_WriteRegister(BMP280_Handle, BMP280_REG_CTRL_MEAS, &Value, 1);
}

HAL_StatusTypeDef BMP280_DummyRead(I2C_HandleTypeDef *BMP280_Handle) {
    return BMP280_ReadRegister(BMP280_Handle, BMP280_REG_PRESS_MSB, data_buffer, BMP280_SENSOR_DATA_SIZE);
}

HAL_StatusTypeDef BMP280_ReadData(I2C_HandleTypeDef *BMP280_Handle, BMP280_SensorData_t *SensorData) {
    if (BMP280_ReadRegister(BMP280_Handle, BMP280_REG_PRESS_MSB, data_buffer, BMP280_SENSOR_DATA_SIZE) != HAL_OK) {
        return HAL_ERROR;
    }

    uint32_t UncompensatedPressure = CalculateUncompensatedPressure(data_buffer);
    uint32_t UncompensatedTemperature = CalculateUncompensatedTemperature(data_buffer);

    int32_t FinalTempRaw = BMP280_CompensateTemperature((int32_t)UncompensatedTemperature);
    uint32_t FinalPressRaw = BMP280_CompensatePressure((int32_t)UncompensatedPressure);

    SensorData->temperature_c = (float)FinalTempRaw / 100.0f;
    SensorData->pressure_pa = (float)FinalPressRaw / 256.0f;

    return HAL_OK;
}
