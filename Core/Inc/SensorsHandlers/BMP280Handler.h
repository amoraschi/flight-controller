#ifndef BMP280HANDLER_H
#define BMP280HANDLER_H

#include "stm32h7xx_hal.h"

HAL_StatusTypeDef BMP280_Mode_Idle(I2C_HandleTypeDef *BMP280_Handle);
HAL_StatusTypeDef BMP280_Mode_Performance(I2C_HandleTypeDef *BMP280_Handle);

#endif //BMP280HANDLER_H
