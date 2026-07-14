#include <stdbool.h>
#include "Sensors/IIM42653.h"
#include "FreeRTOS.h"
#include "Utils/IMU.h"

void IIM42653_SelectCS(void) {
    HAL_GPIO_WritePin(IIM42653_CS_GPIO_PORT, IIM42653_CS_GPIO_PIN, GPIO_PIN_RESET);
}

void IIM42653_DeselectCS(void) {
    HAL_GPIO_WritePin(IIM42653_CS_GPIO_PORT, IIM42653_CS_GPIO_PIN, GPIO_PIN_SET);
}

static HAL_StatusTypeDef IIM42653_WriteRegister(SPI_HandleTypeDef *IIM42653_Handle, uint8_t Reg, uint8_t *Data) {
    uint8_t Buffer[2];

    Buffer[0] = Reg & IIM42653_WRITE_MASK;
    Buffer[1] = *Data;

    IIM42653_SelectCS();
    HAL_StatusTypeDef Status = HAL_SPI_Transmit(IIM42653_Handle, Buffer, 2, 100);
    IIM42653_DeselectCS();

    return Status;
}

static HAL_StatusTypeDef IIM42653_ReadRegister(SPI_HandleTypeDef *IIM42653_Handle, uint8_t Reg, uint8_t *Data) {
    uint8_t TXBuffer[2];
    uint8_t RXBuffer[2] = {0};

    TXBuffer[0] = Reg | IIM42653_READ_MASK;
    TXBuffer[1] = 0x00;

    IIM42653_SelectCS();
    HAL_StatusTypeDef Status = HAL_SPI_TransmitReceive(IIM42653_Handle, TXBuffer, RXBuffer, 2, 100);
    IIM42653_DeselectCS();

    if (Status == HAL_OK) {
        *Data = RXBuffer[1];
    }

    return Status;
}

static HAL_StatusTypeDef IIM42653_SelectBank(SPI_HandleTypeDef *IIM42653_Handle, uint8_t Bank) {
    return IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_REG_BANK_SEL, &Bank);
}

static HAL_StatusTypeDef IIM42653_SetBit(SPI_HandleTypeDef *IIM42653_Handle, uint8_t Reg, uint8_t Flag, bool Set) {
    uint8_t Value = 0;

    if (IIM42653_ReadRegister(IIM42653_Handle, Reg, &Value) != HAL_OK) {
        return HAL_ERROR;
    }

    if (Set) {
        Value |= Flag;
    } else {
        Value &= ~Flag;
    }

    return IIM42653_WriteRegister(IIM42653_Handle, Reg, &Value);
}

static HAL_StatusTypeDef IIM42653_SetBits(SPI_HandleTypeDef *IIM42653_Handle, uint8_t Reg, uint8_t Mask, uint8_t Flags) {
    uint8_t Value = 0;

    if (IIM42653_ReadRegister(IIM42653_Handle, Reg, &Value) != HAL_OK) {
        return HAL_ERROR;
    }

    Value &= ~Mask;
    Value |= (Flags & Mask);

    return IIM42653_WriteRegister(IIM42653_Handle, Reg, &Value);
}

HAL_StatusTypeDef IIM42653_Reset(SPI_HandleTypeDef *IIM42653_Handle) {
    uint8_t ResetCommand = IIM42653_REG_DEVICE_CONFIG_SOFT_RESET_CONFIG;

    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    if (IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_DEVICE_CONFIG, &ResetCommand) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef IIM42653_CheckChipID(SPI_HandleTypeDef *IIM42653_Handle) {
    uint8_t ChipID = 0;

    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    if (IIM42653_ReadRegister(IIM42653_Handle, IIM42653_REG_WHO_AM_I, &ChipID) != HAL_OK) {
        return HAL_ERROR;
    }

    if (ChipID != IIM42653_WHO_AM_I) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef IIM42653_ResetInt(SPI_HandleTypeDef *IIM42653_Handle) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    return IIM42653_SetBit(IIM42653_Handle, IIM42653_REG_INT_CONFIG1, IIM42653_REG_INT_CONFIG1_INT_ASYNC_RESET, false);
}

HAL_StatusTypeDef IIM42653_SetEndianness(SPI_HandleTypeDef *IIM42653_Handle) {
    uint8_t Endianness = IIM42653_REG_INTF_CONFIG0_SENSOR_DATA_ENDIAN | IIM42653_REG_INTF_CONFIG0_FIFO_COUNT_ENDIAN;

    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    return IIM42653_SetBit(IIM42653_Handle, IIM42653_REG_INTF_CONFIG0, Endianness, true);
}

HAL_StatusTypeDef IIM42653_EnableIdle(SPI_HandleTypeDef *IIM42653_Handle) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    return IIM42653_SetBit(IIM42653_Handle, IIM42653_REG_PWR_MGMT0, IIM42653_REG_PWR_MGMT0_IDLE, true);
}

HAL_StatusTypeDef IIM42653_DisableIdle(SPI_HandleTypeDef *IIM42653_Handle) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    return IIM42653_SetBit(IIM42653_Handle, IIM42653_REG_PWR_MGMT0, IIM42653_REG_PWR_MGMT0_IDLE, false);
}

HAL_StatusTypeDef IIM42653_SetGyroConfig(SPI_HandleTypeDef *IIM42653_Handle, uint8_t ODR, uint8_t FS) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t GyroConfig = 0;

    if (IIM42653_ReadRegister(IIM42653_Handle, IIM42653_REG_GYRO_CONFIG0, &GyroConfig) != HAL_OK) {
        return HAL_ERROR;
    }

    GyroConfig &= ~(IIM42653_REG_GYRO_CONFIG0_GYRO_UI_FS_SEL_MASK | IIM42653_REG_GYRO_CONFIG0_GYRO_ODR_MASK);
    GyroConfig |= (FS & IIM42653_REG_GYRO_CONFIG0_GYRO_UI_FS_SEL_MASK) | (ODR & IIM42653_REG_GYRO_CONFIG0_GYRO_ODR_MASK);

    return IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_GYRO_CONFIG0, &GyroConfig);
}

HAL_StatusTypeDef IIM42653_SetAccelConfig(SPI_HandleTypeDef *IIM42653_Handle, uint8_t ODR, uint8_t FS) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t AccelConfig = 0;

    if (IIM42653_ReadRegister(IIM42653_Handle, IIM42653_REG_ACCEL_CONFIG0, &AccelConfig) != HAL_OK) {
        return HAL_ERROR;
    }

    AccelConfig &= ~(IIM42653_REG_ACCEL_CONFIG0_ACCEL_UI_FS_SEL_MASK | IIM42653_REG_ACCEL_CONFIG0_ACCEL_ODR_MASK);
    AccelConfig |= (FS & IIM42653_REG_ACCEL_CONFIG0_ACCEL_UI_FS_SEL_MASK) | (ODR & IIM42653_REG_ACCEL_CONFIG0_ACCEL_ODR_MASK);

    return IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_ACCEL_CONFIG0, &AccelConfig);
}

HAL_StatusTypeDef IIM42653_SetFilterOrder(SPI_HandleTypeDef *IIM42653_Handle, uint8_t AccelOrd, uint8_t GyroOrd) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

	if (IIM42653_SetBits(IIM42653_Handle, IIM42653_REG_ACCEL_CONFIG1, IIM42653_REG_ACCEL_CONFIG1_ACCEL_UI_FILT_ORD_MASK, AccelOrd) != HAL_OK) {
		return HAL_ERROR;
	}

	if (IIM42653_SetBits(IIM42653_Handle, IIM42653_REG_GYRO_CONFIG1, IIM42653_REG_GYRO_CONFIG1_GYRO_UI_FILT_ORD_MASK, GyroOrd) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef IIM42653_SetFilterBandwidth(SPI_HandleTypeDef *IIM42653_Handle, uint8_t AccelBW, uint8_t GyroBW) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t FilterConfig = 0;

    if (IIM42653_ReadRegister(IIM42653_Handle, IIM42653_REG_GYRO_ACCEL_CONFIG0, &FilterConfig) != HAL_OK) {
        return HAL_ERROR;
    }

    FilterConfig &= (uint8_t)(~(IIM42653_REG_GYRO_ACCEL_CONFIG0_ACCEL_UI_FILT_BW_MASK | IIM42653_REG_GYRO_ACCEL_CONFIG0_GYRO_UI_FILT_BW_MASK));
    FilterConfig |= (IIM42653_REG_GYRO_ACCEL_CONFIG0_ACCEL_FILT_OFFSET(AccelBW) & IIM42653_REG_GYRO_ACCEL_CONFIG0_ACCEL_UI_FILT_BW_MASK) | (IIM42653_REG_GYRO_ACCEL_CONFIG0_GYRO_FILT_OFFSET(GyroBW) & IIM42653_REG_GYRO_ACCEL_CONFIG0_GYRO_UI_FILT_BW_MASK);

    return IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_GYRO_ACCEL_CONFIG0, &FilterConfig);
}

HAL_StatusTypeDef IIM42653_SetPowerMode(SPI_HandleTypeDef *IIM42653_Handle, uint8_t AccelMode, uint8_t GyroMode) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t PowerConfig = 0;

    if (IIM42653_ReadRegister(IIM42653_Handle, IIM42653_REG_PWR_MGMT0, &PowerConfig) != HAL_OK) {
        return HAL_ERROR;
    }

    PowerConfig &= ~(IIM42653_REG_PWR_MGMT0_ACCEL_MODE_MASK | IIM42653_REG_PWR_MGMT0_GYRO_MODE_MASK);
    PowerConfig |= ((IIM42653_REG_PWR_MGMT0_GYRO_MODE_OFFSET(GyroMode) & IIM42653_REG_PWR_MGMT0_GYRO_MODE_MASK) | (IIM42653_REG_PWR_MGMT0_ACCEL_MODE_OFFSET(AccelMode) & IIM42653_REG_PWR_MGMT0_ACCEL_MODE_MASK));

    return IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_PWR_MGMT0, &PowerConfig);
}

HAL_StatusTypeDef IIM42653_SetGyroAAF(SPI_HandleTypeDef *IIM42653_Handle, uint8_t Delt, uint16_t DeltSqr, uint8_t BitShift) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (IIM42653_SetBits(IIM42653_Handle, IIM42653_REG_GYRO_CONFIG_STATIC3, IIM42653_REG_GYRO_CONFIG_STATIC3_AAF_DELT_MASK, Delt) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t DeltSqrLow = (uint8_t)(DeltSqr & 0xFF);
    if (IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_GYRO_CONFIG_STATIC4, &DeltSqrLow) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t BitShiftAndDeltSqrHigh = (BitShift << 4) & IIM42653_REG_GYRO_CONFIG_STATIC5_AAF_BITSHIFT_MASK;
    BitShiftAndDeltSqrHigh |= (DeltSqr >> 8) & IIM42653_REG_GYRO_CONFIG_STATIC5_AAF_DELTSQR_MASK;
    if (IIM42653_WriteRegister(IIM42653_Handle, IIM42653_REG_GYRO_CONFIG_STATIC5, &BitShiftAndDeltSqrHigh) != HAL_OK) {
        return HAL_ERROR;
    }

    return IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0);
}

HAL_StatusTypeDef IIM42653_DisableNotchFilter(SPI_HandleTypeDef *IIM42653_Handle) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_1) != HAL_OK) {
        return HAL_ERROR;
    }

    if (IIM42653_SetBit(IIM42653_Handle, IIM42653_REG_GYRO_CONFIG_STATIC2, IIM42653_REG_GYRO_CONFIG_STATIC2_GYRO_NF_DIS, true) != HAL_OK) {
        return HAL_ERROR;
    }

    return IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0);
}

HAL_StatusTypeDef IIM42653_EnableFIFOHold(SPI_HandleTypeDef *IIM42653_Handle) {
  if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
      return HAL_ERROR;
  }

	return IIM42653_SetBit(IIM42653_Handle, IIM42653_REG_INTF_CONFIG0, IIM42653_REG_INTF_CONFIG0_FIFO_HOLD_LAST_DATA_EN, true);
}

HAL_StatusTypeDef IIM42653_EnableDataReady(SPI_HandleTypeDef *IIM42653_Handle) {
    if (IIM42653_SelectBank(IIM42653_Handle, IIM42653_USER_BANK_0) != HAL_OK) {
        return HAL_ERROR;
    }

    return IIM42653_SetBit(IIM42653_Handle, IIM42653_REG_INT_SOURCE0, IIM42653_REG_INT_SOURCE0_UI_DRDY_INT1_EN, true);
}

HAL_StatusTypeDef IIM42653_CheckDataReady(SPI_HandleTypeDef *IIM42653_Handle, bool *Ready) {
    uint8_t Status = 0;

    if (IIM42653_ReadRegister(IIM42653_Handle, IIM42653_REG_INT_STATUS, &Status) != HAL_OK) {
        return HAL_ERROR;
    }

    *Ready = (Status & IIM42653_REG_INT_STATUS_DATA_RDY_INT) != 0;

    return HAL_OK;
}

HAL_StatusTypeDef IIM42653_GetData(SPI_HandleTypeDef *IIM42653_Handle, IIM42653_SensorData_t *SensorData) {
    HAL_StatusTypeDef Status;

    uint8_t TXBuffer[13] = {0};
    uint8_t RXBuffer[13] = {0};

    TXBuffer[0] = IIM42653_REG_ACCEL_DATA_X1_UI | IIM42653_READ_MASK;

    IIM42653_SelectCS();
    Status = HAL_SPI_TransmitReceive(IIM42653_Handle, TXBuffer, RXBuffer, IIM42653_SENSOR_DATA_SIZE, 100);
    IIM42653_DeselectCS();

    SensorData->AccelX = CalculateAcceleration(RXBuffer[1], RXBuffer[2], ACCEL_SCALE);
    SensorData->AccelY = CalculateAcceleration(RXBuffer[3], RXBuffer[4], ACCEL_SCALE);
    SensorData->AccelZ = CalculateAcceleration(RXBuffer[5], RXBuffer[6], ACCEL_SCALE);
    SensorData->GyroX  = CalculateGyroscope(RXBuffer[7], RXBuffer[8], GYRO_SCALE);
    SensorData->GyroY  = CalculateGyroscope(RXBuffer[9], RXBuffer[10], GYRO_SCALE);
    SensorData->GyroZ  = CalculateGyroscope(RXBuffer[11], RXBuffer[12], GYRO_SCALE);

    return Status;
}
