#ifndef IIM42653HANDLER_H
#define IIM42653HANDLER_H

#include "stm32h7xx_hal.h"

HAL_StatusTypeDef IIM42653_Mode_Idle(SPI_HandleTypeDef *IIM42653_Handle);
HAL_StatusTypeDef IIM42653_Mode_Performance(SPI_HandleTypeDef *IIM42653_Handle);

#endif //IIM42653HANDLER_H
