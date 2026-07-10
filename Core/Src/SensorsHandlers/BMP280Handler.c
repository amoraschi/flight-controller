#include "SensorsHandlers/BMP280Handler.h"
#include "Sensors/BMP280.h"
#include "FreeRTOS.h"
#include "task.h"

HAL_StatusTypeDef BMP280_Mode_Idle(I2C_HandleTypeDef *BMP280_Handle) {
    if (BMP280_Handle == NULL) {
        return HAL_ERROR;
    }

    if (BMP280_Reset(BMP280_Handle) != HAL_OK) {
        return HAL_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (BMP280_CheckChipID(BMP280_Handle) != HAL_OK) {
        return HAL_ERROR;
    }

    if (BMP280_ReadCalibrationData(BMP280_Handle) != HAL_OK) {
        return HAL_ERROR;
    }

    if (BMP280_SetCtrlMeas(BMP280_Handle, BMP280_MODE_SLEEP) != HAL_OK) {
        return HAL_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (BMP280_DummyRead(BMP280_Handle) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef BMP280_Mode_Performance(I2C_HandleTypeDef *BMP280_Handle) {
    if (BMP280_Handle == NULL) {
        return HAL_ERROR;
    }

    if (BMP280_SetConfig(BMP280_Handle, BMP280_STANDBY_MS_0_5 | BMP280_FILTER_COEFF_16) != HAL_OK) {
        return HAL_ERROR;
    }

    if (BMP280_SetCtrlMeas(BMP280_Handle, BMP280_TEMP_OVERSAMPLING_X2 | BMP280_PRESS_OVERSAMPLING_X16 | BMP280_MODE_NORMAL) != HAL_OK) {
        return HAL_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (BMP280_DummyRead(BMP280_Handle) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}
