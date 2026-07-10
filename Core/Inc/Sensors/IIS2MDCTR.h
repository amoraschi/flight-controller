#ifndef IIS2MDCTR_H
#define IIS2MDCTR_H

#include "stm32h7xx_hal.h"
#include <stdbool.h>

#define IIS2MDCTR_I2C_ADDRESS           (0x1E << 1)
#define IIS2MDCTR_SENSOR_DATA_SIZE      6
#define IIS2MDCTR_AUTO_INCREMENT_MASK   0x80

// REG_WHO_AM_I
#define IIS2MDCTR_REG_WHO_AM_I                  0x4F
#define IIS2MDCTR_WHO_AM_I                      0x40

// REG_CFG_REG_A
#define IIS2MDCTR_REG_CFG_REG_A                 0x60
#define IIS2MDCTR_REG_CFG_REG_A_COMP_TEMP_EN    (0x01 << 7)
#define IIS2MDCTR_REG_CFG_REG_A_SOFT_RST        (0x01 << 5)
#define IIS2MDCTR_REG_CFG_REG_A_LP              (0x01 << 4)

#define IIS2MDCTR_REG_CFG_REG_A_ODR_MASK        (0x03 << 2)
#define IIS2MDCTR_ODR_HZ_10                     (0x00 << 2)
#define IIS2MDCTR_ODR_HZ_20                     (0x01 << 2)
#define IIS2MDCTR_ODR_HZ_50                     (0x02 << 2)
#define IIS2MDCTR_ODR_HZ_100                    (0x03 << 2)

#define IIS2MDCTR_REG_CFG_REG_A_MD_MASK         (0x03 << 0)
#define IIS2MDCTR_MD_CONTINUOUS                 (0x00 << 0)
#define IIS2MDCTR_MD_SINGLE                     (0x01 << 0)
#define IIS2MDCTR_MD_IDLE                       (0x03 << 0)

// REG_CFG_REG_B
#define IIS2MDCTR_REG_CFG_REG_B                 0x61
#define IIS2MDCTR_REG_CFG_REG_B_SET_FREQ        (0x01 << 2)
#define IIS2MDCTR_REG_CFG_REG_B_OFF_CANC        (0x01 << 1)
#define IIS2MDCTR_REG_CFG_REG_B_LPF             (0x01 << 0)

// REG_CFG_REG_C
#define IIS2MDCTR_REG_CFG_REG_C                 0x62
#define IIS2MDCTR_REG_CFG_REG_C_BDU             (0x01 << 4)

// REG_OUTX_L_REG
#define IIS2MDCTR_REG_OUTX_L_REG                0x68

typedef struct {
    float MagX;
    float MagY;
    float MagZ;
} IIS2MDCTR_SensorData_t;

HAL_StatusTypeDef IIS2MDCTR_ReadRegister(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t *Data, uint16_t Size);
HAL_StatusTypeDef IIS2MDCTR_WriteRegister(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t *Data, uint16_t Size);
HAL_StatusTypeDef IIS2MDCTR_SetBit(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t Flag, bool Set);
HAL_StatusTypeDef IIS2MDCTR_SetBits(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t Mask, uint8_t Flags);
HAL_StatusTypeDef IIS2MDCTR_Reset(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_CheckChipID(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_EnableTempComp(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_SetMode(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Mode);
HAL_StatusTypeDef IIS2MDCTR_EnableLP(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_DisableLP(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_EnableBDU(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_EnablePulseFrequency(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_DisablePulseFrequency(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_EnableOffsetCancellation(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_DisableOffsetCancellation(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_SetODR(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t ODR);
HAL_StatusTypeDef IIS2MDCTR_EnableLPF(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_GetData(I2C_HandleTypeDef *IIS2MDCTR_Handle, IIS2MDCTR_SensorData_t *SensorData);

#endif //IIS2MDCTR_H
