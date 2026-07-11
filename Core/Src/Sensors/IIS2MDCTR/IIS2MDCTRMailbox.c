#include "Sensors/IIS2MDCTR.h"
#include "Utils/Magnetometer.h"
#include "Sensors/Sensors.h"

static IIS2MDCTR_Mailbox_t IIS2MDCTR_Mailbox = {0};

volatile uint8_t dbg_slot;

void IIS2MDCTR_Mailbox_Publish(const uint8_t *RXBuffer) {
    uint8_t wi = IIS2MDCTR_Mailbox.WriteIndex;
    IIS2MDCTR_Mailbox.Slot[wi].MagX = CalculateMagneticField(RXBuffer[1], RXBuffer[0]);
    IIS2MDCTR_Mailbox.Slot[wi].MagY = CalculateMagneticField(RXBuffer[3], RXBuffer[2]);
    IIS2MDCTR_Mailbox.Slot[wi].MagZ = CalculateMagneticField(RXBuffer[5], RXBuffer[4]);

    IIS2MDCTR_Mailbox.WriteIndex = 1 - wi;
}

void IIS2MDCTR_Mailbox_Read(IIS2MDCTR_SensorData_t *Out) {
    uint8_t ri = 1 - IIS2MDCTR_Mailbox.WriteIndex;
    dbg_slot = ri;
    *Out = IIS2MDCTR_Mailbox.Slot[ri];
}
