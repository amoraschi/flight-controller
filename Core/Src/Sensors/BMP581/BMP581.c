#include "Sensors/BMP581.h"
#include "Utils/Calculations.h"
#include "Utils/shared.h"

static HAL_StatusTypeDef BMP581_ReadRegister(I2C_HandleTypeDef *BMP581_Handle, uint8_t Reg, uint8_t *Data, uint16_t Size) {
    if (BMP581_Handle == NULL || Data == NULL) {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Read(BMP581_Handle, BMP581_I2C_ADDRESS, Reg, I2C_MEMADD_SIZE_8BIT, Data, Size, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

static HAL_StatusTypeDef BMP581_WriteRegister(I2C_HandleTypeDef *BMP581_Handle, uint8_t Reg, uint8_t *Data, uint16_t Size) {
    if (BMP581_Handle == NULL || Data == NULL) {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Write(BMP581_Handle, BMP581_I2C_ADDRESS, Reg, I2C_MEMADD_SIZE_8BIT, Data, Size, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

static HAL_StatusTypeDef BMP581_SetBit(I2C_HandleTypeDef *BMP581_Handle, uint8_t Reg, uint8_t Flag, bool Set) {
    uint8_t Value = 0;

    if (BMP581_ReadRegister(BMP581_Handle, Reg, &Value, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (Set) {
        Value |= Flag;
    } else {
        Value &= ~Flag;
    }

    return BMP581_WriteRegister(BMP581_Handle, Reg, &Value, 1);
}

static HAL_StatusTypeDef BMP581_SetBits(I2C_HandleTypeDef *BMP581_Handle, uint8_t Reg, uint8_t Mask, uint8_t Flags) {
    uint8_t Value = 0;

    if (BMP581_ReadRegister(BMP581_Handle, Reg, &Value, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    Value &= ~Mask;
    Value |= (Flags & Mask);

    return BMP581_WriteRegister(BMP581_Handle, Reg, &Value, 1);
}

HAL_StatusTypeDef BMP581_Reset(I2C_HandleTypeDef *BMP581_Handle) {
    uint8_t ResetCmd = BMP581_RESET;
    return BMP581_WriteRegister(BMP581_Handle, BMP581_REG_CMD, &ResetCmd, 1);
}

HAL_StatusTypeDef BMP581_CheckChipID(I2C_HandleTypeDef *BMP581_Handle) {
    uint8_t ChipID = 0;

    if (BMP581_ReadRegister(BMP581_Handle, BMP581_REG_CHIP_ID, &ChipID, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (ChipID != BMP581_ID) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef BMP581_CheckNVMReady(I2C_HandleTypeDef *BMP581_Handle) {
    uint8_t Status = 0;

    if (BMP581_ReadRegister(BMP581_Handle, BMP581_REG_STATUS, &Status, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (BITS_SET(Status, BMP581_REG_STATUS_NVM_ERR) || BITS_CLEAR(Status, BMP581_REG_STATUS_NVM_RDY)) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef BMP581_CheckPOR(I2C_HandleTypeDef *BMP581_Handle) {
    uint8_t IntStatus = 0;

    if (BMP581_ReadRegister(BMP581_Handle, BMP581_REG_INT_STATUS, &IntStatus, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (BITS_CLEAR(IntStatus, BMP581_REG_INT_STATUS_POR)) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef BMP581_EnableDeepStandby(I2C_HandleTypeDef *BMP581_Handle) {
    return BMP581_SetBit(BMP581_Handle, BMP581_REG_ODR_CONFIG, BMP581_REG_ODR_CONFIG_DEEP_DIS, false);
}

HAL_StatusTypeDef BMP581_DisableDeepStandby(I2C_HandleTypeDef *BMP581_Handle) {
    return BMP581_SetBit(BMP581_Handle, BMP581_REG_ODR_CONFIG, BMP581_REG_ODR_CONFIG_DEEP_DIS, true);
}

HAL_StatusTypeDef BMP581_DisableFIFO(I2C_HandleTypeDef *BMP581_Handle) {
    return BMP581_SetBit(BMP581_Handle, BMP581_REG_FIFO_SEL, BMP581_REG_FIFO_SEL_FIFO_FRAME_SEL, false);
}

HAL_StatusTypeDef BMP581_DisableIIR(I2C_HandleTypeDef *BMP581_Handle) {
    return BMP581_SetBit(BMP581_Handle, BMP581_REG_DSP_IIR, BMP581_REG_DSP_IIR_TEMP_COEFFICIENT_OFFSET(BMP581_REG_DSP_IIR_COEFFICIENT_X0) | BMP581_REG_DSP_IIR_PRESS_COEFFICIENT_OFFSET(BMP581_REG_DSP_IIR_COEFFICIENT_X0), false);
}

HAL_StatusTypeDef BMP581_SetIIR(I2C_HandleTypeDef *BMP581_Handle, uint8_t PressureIIR, uint8_t TemperatureIIR) {
    return BMP581_SetBits(BMP581_Handle, BMP581_REG_DSP_IIR, BMP581_REG_DSP_IIR_TEMP_COEFFICIENT_OFFSET(TemperatureIIR) | BMP581_REG_DSP_IIR_PRESS_COEFFICIENT_OFFSET(PressureIIR), BMP581_REG_DSP_IIR_SET_IIR_T_MASK | BMP581_REG_DSP_IIR_SET_IIR_P_MASK);
}

HAL_StatusTypeDef BMP581_EnableCompensation(I2C_HandleTypeDef *BMP581_Handle) {
    return BMP581_SetBit(BMP581_Handle, BMP581_REG_DSP_CONFIG, BMP581_REG_DSP_CONFIG_COMP_PT_EN | BMP581_REG_DSP_CONFIG_SHDW_SEL_IIR_T | BMP581_REG_DSP_CONFIG_SHDW_SEL_IIR_P, true);
}

HAL_StatusTypeDef BMP581_SetPowerMode(I2C_HandleTypeDef *BMP581_Handle, uint8_t PowerMode) {
    return BMP581_SetBits(BMP581_Handle, BMP581_REG_ODR_CONFIG, BMP581_REG_ODR_CONFIG_PWR_MODE_MASK, PowerMode);
}

HAL_StatusTypeDef BMP581_SetODR(I2C_HandleTypeDef *BMP581_Handle, uint8_t ODR) {
    return BMP581_SetBits(BMP581_Handle, BMP581_REG_ODR_CONFIG, BMP581_REG_ODR_CONFIG_ODR_MASK, ODR);
}

HAL_StatusTypeDef BMP581_SetOSR(I2C_HandleTypeDef *BMP581_Handle, uint8_t PressureOSR, uint8_t TemperatureOSR) {
    uint8_t OSRConfig = 0;

    if (BMP581_ReadRegister(BMP581_Handle, BMP581_REG_OSR_CONFIG, &OSRConfig, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    OSRConfig &= ~(BMP581_REG_OSR_CONFIG_OSR_T_MASK | BMP581_REG_OSR_CONFIG_OSR_P_MASK);
    OSRConfig |= (BMP581_REG_OSR_CONFIG_TEMP_OVERSAMPLING_OFFSET(TemperatureOSR) & BMP581_REG_OSR_CONFIG_OSR_T_MASK) | (BMP581_REG_OSR_CONFIG_PRESS_OVERSAMPLING_OFFSET(PressureOSR) & BMP581_REG_OSR_CONFIG_OSR_P_MASK);

    return BMP581_WriteRegister(BMP581_Handle, BMP581_REG_OSR_CONFIG, &OSRConfig, 1);
}

HAL_StatusTypeDef BMP581_EnablePressure(I2C_HandleTypeDef *BMP581_Handle) {
    return BMP581_SetBit(BMP581_Handle, BMP581_REG_OSR_CONFIG, BMP581_REG_OSR_CONFIG_PRESS_EN, true);
}

HAL_StatusTypeDef BMP581_GetData(I2C_HandleTypeDef *BMP581_Handle, BMP581_SensorData_t *SensorData) {
    uint8_t DataBuffer[BMP581_SENSOR_DATA_SIZE] = {0};

    if (BMP581_ReadRegister(BMP581_Handle, BMP581_REG_TEMPERATURE_XLSB, DataBuffer, BMP581_SENSOR_DATA_SIZE) != HAL_OK) {
        return HAL_ERROR;
    }

    SensorData->TemperatureC = CalculatePressureTemperature(DataBuffer[2], DataBuffer[1], DataBuffer[0], true);
    SensorData->PressurePa = CalculatePressureTemperature(DataBuffer[5], DataBuffer[4], DataBuffer[3], false);

    return HAL_OK;
}
