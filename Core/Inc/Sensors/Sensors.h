#ifndef MAILBOXES_H
#define MAILBOXES_H

#include "Sensors/IIS2MDCTR.h"
#include "Sensors/IIM42653.h"
#include "Sensors/BMP581.h"

typedef struct {
    BMP581_SensorData_t Slot[2];
    volatile uint8_t WriteIndex;
} BMP581_Mailbox_t;

typedef struct {
    IIM42653_SensorData_t Slot[2];
    volatile uint8_t WriteIndex;
} IIM42653_Mailbox_t;

typedef struct {
    IIS2MDCTR_SensorData_t Slot[2];
    volatile uint8_t WriteIndex;
} IIS2MDCTR_Mailbox_t;

void BUZZER_Beep(uint32_t Duration);

void BMP581_Mailbox_Publish(const uint8_t *RXBuffer);
void BMP581_Mailbox_Read(BMP581_SensorData_t *Out);
void IIM42653_Mailbox_Publish(const uint8_t *RXBuffer);
void IIM42653_Mailbox_Read(IIM42653_SensorData_t *Out);
void IIS2MDCTR_Mailbox_Publish(const uint8_t *RXBuffer);
void IIS2MDCTR_Mailbox_Read(IIS2MDCTR_SensorData_t *Out);

HAL_StatusTypeDef BMP581_Mode_Idle(I2C_HandleTypeDef *BMP581_Handle);
HAL_StatusTypeDef BMP581_Mode_Performance(I2C_HandleTypeDef *BMP581_Handle);
HAL_StatusTypeDef IIM42653_Mode_Idle(SPI_HandleTypeDef *IIM42653_Handle);
HAL_StatusTypeDef IIM42653_Mode_Performance(SPI_HandleTypeDef *IIM42653_Handle);
HAL_StatusTypeDef IIS2MDCTR_Mode_Idle(I2C_HandleTypeDef *IIS2MDCTR_Handle);
HAL_StatusTypeDef IIS2MDCTR_Mode_Performance(I2C_HandleTypeDef *IIS2MDCTR_Handle);

#endif // MAILBOXES_H
