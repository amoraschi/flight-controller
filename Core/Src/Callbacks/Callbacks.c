#include <FreeRTOS.h>
#include <stm32h7xx_hal.h>
#include <Sensors/Sensors.h>
#include <task.h>
#include <timers.h>
#include <Tasks/TelemetryTask.h>
#include "Sensors/BMP581.h"
#include "Sensors/IIM42653.h"
#include "Sensors/IIS2MDCTR.h"
#include "Utils/shared.h"
#include "Utils/Diagnostics.h"

__attribute__((section(".dma_buffer"), aligned(32)))
uint8_t IIM42653_TXBuf[IIM42653_SENSOR_DATA_SIZE];

__attribute__((section(".dma_buffer"), aligned(32)))
uint8_t IIM42653_RXBuf[IIM42653_SENSOR_DATA_SIZE];

__attribute__((section(".dma_buffer"), aligned(32)))
uint8_t BMP581_RXBuf[BMP581_SENSOR_DATA_SIZE];

__attribute__((section(".dma_buffer"), aligned(32)))
uint8_t IIS2MDCTR_RXBuf[IIS2MDCTR_SENSOR_DATA_SIZE];

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        if (TelemetryTaskHandle != NULL) {
            xTaskNotifyFromISR(TelemetryTaskHandle, Size, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        HAL_UART_AbortReceive_IT(huart);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, TELEMETRY_RX_BUFFER, TELEMETRY_RX_BUFFER_SIZE);
    }
}

void IIM42653_Timer_Callback(TimerHandle_t xTimer) {
	bool Ready = false;
	IIM42653_CheckDataReady(IIM42653_HANDLE, &Ready);
	if (Ready) {
		DiagnosticsSensorStart(DIAG_SENSOR_IIM42653);
		IIM42653_TXBuf[0] = IIM42653_REG_ACCEL_DATA_X1_UI | IIM42653_READ_MASK;
		IIM42653_SelectCS();
		if (HAL_SPI_TransmitReceive_DMA(IIM42653_HANDLE, IIM42653_TXBuf, IIM42653_RXBuf, IIM42653_SENSOR_DATA_SIZE) != HAL_OK) {
			DiagnosticsSensorError(DIAG_SENSOR_IIM42653);
			IIM42653_DeselectCS();
		}
	}
}

void BMP581_Timer_Callback(TimerHandle_t xTimer) {
    DiagnosticsSensorStart(DIAG_SENSOR_BMP581);
    if (HAL_I2C_Mem_Read_DMA(BMP581_HANDLE, BMP581_I2C_ADDRESS, BMP581_REG_TEMPERATURE_XLSB, I2C_MEMADD_SIZE_8BIT, BMP581_RXBuf, BMP581_SENSOR_DATA_SIZE) != HAL_OK) {
        DiagnosticsSensorError(DIAG_SENSOR_BMP581);
    }
}

void IIS2MDCTR_Timer_Callback(TimerHandle_t xTimer) {
    DiagnosticsSensorStart(DIAG_SENSOR_IIS2MDCTR);
    if (HAL_I2C_Mem_Read_DMA(IIS2MDCTR_HANDLE, IIS2MDCTR_I2C_ADDRESS, IIS2MDCTR_REG_OUTX_L_REG | IIS2MDCTR_AUTO_INCREMENT_MASK, I2C_MEMADD_SIZE_8BIT, IIS2MDCTR_RXBuf, IIS2MDCTR_SENSOR_DATA_SIZE) != HAL_OK) {
        DiagnosticsSensorError(DIAG_SENSOR_IIS2MDCTR);
    }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI2) {
        DiagnosticsSensorError(DIAG_SENSOR_IIM42653);
        IIM42653_DeselectCS();
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C2) {
        DiagnosticsSensorError(DIAG_SENSOR_BMP581);
    } else if (hi2c->Instance == I2C1) {
        DiagnosticsSensorError(DIAG_SENSOR_IIS2MDCTR);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI2) {
        DiagnosticsSensorEnd(DIAG_SENSOR_IIM42653);
        IIM42653_DeselectCS();
        IIM42653_Mailbox_Publish(IIM42653_RXBuf);
    }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C2) {
        DiagnosticsSensorEnd(DIAG_SENSOR_BMP581);
        BMP581_Mailbox_Publish(BMP581_RXBuf);
    } else if (hi2c->Instance == I2C1) {
        DiagnosticsSensorEnd(DIAG_SENSOR_IIS2MDCTR);
        IIS2MDCTR_Mailbox_Publish(IIS2MDCTR_RXBuf);
    }
}
