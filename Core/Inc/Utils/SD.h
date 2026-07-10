#ifndef SD_H
#define SD_H

#include "fatfs.h"
#include "Utils/shared.h"

typedef struct {
    SensorData_t Records[SD_LOGGING_RECORDS_PER_BUFFER];
    uint16_t Count;
} SDLoggingBuffer_t;

FRESULT MountAndOpen(void);
void CloseFile(void);
FRESULT WriteLoggingBuffer(SDLoggingBuffer_t *Buffer);

#endif //SD_H
