#include "Sensors/IIS2MDCTR.h"

#include "Utils/Magnetometer.h"

HAL_StatusTypeDef IIS2MDCTR_ReadRegister(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t *Data, uint16_t Size) {
    if (IIS2MDCTR_Handle == NULL || Data == NULL) {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Read(IIS2MDCTR_Handle, IIS2MDCTR_I2C_ADDRESS, Reg, I2C_MEMADD_SIZE_8BIT, Data, Size, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef IIS2MDCTR_WriteRegister(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t *Data, uint16_t Size) {
    if (IIS2MDCTR_Handle == NULL || Data == NULL) {
        return HAL_ERROR;
    }

    if (HAL_I2C_Mem_Write(IIS2MDCTR_Handle, IIS2MDCTR_I2C_ADDRESS, Reg, I2C_MEMADD_SIZE_8BIT, Data, Size, 100) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef IIS2MDCTR_SetBit(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t Flag, bool Set) {
    uint8_t Value = 0;

    if (IIS2MDCTR_ReadRegister(IIS2MDCTR_Handle, Reg, &Value, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (Set) {
        Value |= Flag;
    } else {
        Value &= ~Flag;
    }

    return IIS2MDCTR_WriteRegister(IIS2MDCTR_Handle, Reg, &Value, 1);
}

HAL_StatusTypeDef IIS2MDCTR_SetBits(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Reg, uint8_t Mask, uint8_t Flags) {
    uint8_t Value = 0;

    if (IIS2MDCTR_ReadRegister(IIS2MDCTR_Handle, Reg, &Value, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    Value &= ~Mask;
    Value |= (Flags & Mask);

    return IIS2MDCTR_WriteRegister(IIS2MDCTR_Handle, Reg, &Value, 1);
}

HAL_StatusTypeDef IIS2MDCTR_Reset(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_A, IIS2MDCTR_REG_CFG_REG_A_SOFT_RST, true);
}

HAL_StatusTypeDef IIS2MDCTR_CheckChipID(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    uint8_t ChipID = 0;

    if (IIS2MDCTR_ReadRegister(IIS2MDCTR_Handle, IIS2MDCTR_REG_WHO_AM_I, &ChipID, 1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (ChipID != IIS2MDCTR_WHO_AM_I) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef IIS2MDCTR_EnableTempComp(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_A, IIS2MDCTR_REG_CFG_REG_A_COMP_TEMP_EN, true);
}

HAL_StatusTypeDef IIS2MDCTR_SetMode(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t Mode) {
    return IIS2MDCTR_SetBits(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_A, IIS2MDCTR_REG_CFG_REG_A_MD_MASK, Mode);
}

HAL_StatusTypeDef IIS2MDCTR_EnableLP(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_A, IIS2MDCTR_REG_CFG_REG_A_LP, true);
}

HAL_StatusTypeDef IIS2MDCTR_DisableLP(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_A, IIS2MDCTR_REG_CFG_REG_A_LP, false);
}

HAL_StatusTypeDef IIS2MDCTR_EnableBDU(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_C, IIS2MDCTR_REG_CFG_REG_C_BDU, true);
}

HAL_StatusTypeDef IIS2MDCTR_EnablePulseFrequency(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_B, IIS2MDCTR_REG_CFG_REG_B_SET_FREQ, false);
}

HAL_StatusTypeDef IIS2MDCTR_DisablePulseFrequency(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_B, IIS2MDCTR_REG_CFG_REG_B_SET_FREQ, true);
}

HAL_StatusTypeDef IIS2MDCTR_EnableOffsetCancellation(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_B, IIS2MDCTR_REG_CFG_REG_B_OFF_CANC, true);
}

HAL_StatusTypeDef IIS2MDCTR_DisableOffsetCancellation(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_B, IIS2MDCTR_REG_CFG_REG_B_OFF_CANC, false);
}

HAL_StatusTypeDef IIS2MDCTR_SetODR(I2C_HandleTypeDef *IIS2MDCTR_Handle, uint8_t ODR) {
    return IIS2MDCTR_SetBits(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_A, IIS2MDCTR_REG_CFG_REG_A_ODR_MASK, ODR);
}

HAL_StatusTypeDef IIS2MDCTR_EnableLPF(I2C_HandleTypeDef *IIS2MDCTR_Handle) {
    return IIS2MDCTR_SetBit(IIS2MDCTR_Handle, IIS2MDCTR_REG_CFG_REG_B, IIS2MDCTR_REG_CFG_REG_B_LPF, true);
}

HAL_StatusTypeDef IIS2MDCTR_GetData(I2C_HandleTypeDef *IIS2MDCTR_Handle, IIS2MDCTR_SensorData_t *SensorData) {
    uint8_t DataBuffer[IIS2MDCTR_SENSOR_DATA_SIZE] = {0};

    if (IIS2MDCTR_ReadRegister(IIS2MDCTR_Handle, IIS2MDCTR_REG_OUTX_L_REG | IIS2MDCTR_AUTO_INCREMENT_MASK, DataBuffer, IIS2MDCTR_SENSOR_DATA_SIZE) != HAL_OK) {
        return HAL_ERROR;
    }

    SensorData->MagX = CalculateMagneticField(DataBuffer[1], DataBuffer[0]);
    SensorData->MagY = CalculateMagneticField(DataBuffer[3], DataBuffer[2]);
    SensorData->MagZ = CalculateMagneticField(DataBuffer[5], DataBuffer[4]);

    return HAL_OK;
}
