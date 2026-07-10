#ifndef BMP581HANDLER_H
#define BMP581HANDLER_H

#include "stm32h7xx_hal.h"

HAL_StatusTypeDef BMP581_Mode_Idle(I2C_HandleTypeDef *BMP581_Handle);
HAL_StatusTypeDef BMP581_Mode_Performance(I2C_HandleTypeDef *BMP581_Handle);

#endif //BMP581HANDLER_H
