#include "Sensors/IIM42653.h"
#include "Utils/Calculations.h"
#include "Sensors/Sensors.h"

static IIM42653_Mailbox_t IIM42653_Mailbox = {0};

void IIM42653_Mailbox_Publish(const uint8_t *RXBuffer) {
    uint8_t wi = IIM42653_Mailbox.WriteIndex;
    IIM42653_Mailbox.Slot[wi].AccelX = CalculateAcceleration(RXBuffer[1], RXBuffer[2], ACCEL_SCALE);
    IIM42653_Mailbox.Slot[wi].AccelY = CalculateAcceleration(RXBuffer[3], RXBuffer[4], ACCEL_SCALE);
    IIM42653_Mailbox.Slot[wi].AccelZ = CalculateAcceleration(RXBuffer[5], RXBuffer[6], ACCEL_SCALE);
    IIM42653_Mailbox.Slot[wi].GyroX  = CalculateGyroscope(RXBuffer[7], RXBuffer[8], GYRO_SCALE);
    IIM42653_Mailbox.Slot[wi].GyroY  = CalculateGyroscope(RXBuffer[9], RXBuffer[10], GYRO_SCALE);
    IIM42653_Mailbox.Slot[wi].GyroZ  = CalculateGyroscope(RXBuffer[11], RXBuffer[12], GYRO_SCALE);

    IIM42653_Mailbox.WriteIndex = 1 - wi;
}

void IIM42653_Mailbox_Inject(const IIM42653_SensorData_t *Data) {
    uint8_t wi = IIM42653_Mailbox.WriteIndex;
    IIM42653_Mailbox.Slot[wi] = *Data;
    IIM42653_Mailbox.WriteIndex = 1 - wi;
}

void IIM42653_Mailbox_Read(IIM42653_SensorData_t *Out) {
    uint8_t ri = 1 - IIM42653_Mailbox.WriteIndex;
    *Out = IIM42653_Mailbox.Slot[ri];
}
