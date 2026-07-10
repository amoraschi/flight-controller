#ifndef IIS2MDCTRHANDLER_H
#define IIS2MDCTRHANDLER_H

#include "stm32h7xx_hal.h"

HAL_StatusTypeDef IIS2MDCTR_Mode_Idle(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_Mode_Performance(I2C_HandleTypeDef *IIS2MDCTR_Handle);

#endif //IIS2MDCTRHANDLER_H
