#include "Tasks/vSensorTask.h"

#include <string.h>

#include "Utils/Altitude.h"
#include "Utils/IMU.h"
#include "Utils/configuration.h"
#include "Utils/shared.h"
#include "Sensors/BMP280.h"
#include "Utils/HIL.h"
#include "Utils/Velocity.h"
#include "SensorsHandlers/BMP280Handler.h"
#include "Sensors/BMP581.h"
#include "SensorsHandlers/BMP581Handler.h"
#include "Sensors/IIM42653.h"
#include "SensorsHandlers/IIM42653Handler.h"
#include "Sensors/IIS2MDCTR.h"
#include "SensorsHandlers/IIS2MDCTRHandler.h"

TaskHandle_t SensorTaskHandle;

volatile uint32_t dbg_sensor_task_counter = 0;

void vCreateSensorTask(SensorTaskContext_t *TaskContext, UBaseType_t Priority, const uint16_t StackSize) {
    xTaskCreate(
        vSensorTask,
        "SENSOR_TASK",
        StackSize,
        TaskContext,
        Priority,
        &SensorTaskHandle
    );
}

// TODO: Test code, remove later
void vApplyMode(SystemContext_t *xSystemContext, SensorMode_t mode, SystemHandle_t xSystemHandle) {
    switch (mode) {
        case SENSOR_MODE_IDLE:
            // if (BMP280_Mode_Idle(xSystemHandle.BMP280_Handle) != HAL_OK) {
            //     xSystemFaultFlags.Flags |= BMP280_MODE_IDLE_FAILED;
            // }
            if (BMP581_Mode_Idle(xSystemHandle.BMP581_Handle) != HAL_OK) {
                xSystemFaultFlags.Flags |= BMP581_MODE_IDLE_FAILED;
            }
            if (IIM42653_Mode_Idle(xSystemHandle.IIM42653_Handle) != HAL_OK) {
                xSystemFaultFlags.Flags |= IIM42653_MODE_IDLE_FAILED;
            }
            if (IIS2MDCTR_Mode_Idle(xSystemHandle.IIS2MDCTR_Handle) != HAL_OK) {
                xSystemFaultFlags.Flags |= IIS2MDCTR_MODE_IDLE_FAILED;
            }
            // TODO: Test code, remove later
#if AUTO_START_CALIBRATION
            xSystemContext->SensorsIdleFinished = true;
#endif
            break;
        case SENSOR_MODE_PERFORMANCE:
            // if (BMP280_Mode_Performance(xSystemHandle.BMP280_Handle) != HAL_OK) {
            //     xSystemFaultFlags.Flags |= BMP280_MODE_PERFORMANCE_FAILED;
            // }
            if (BMP581_Mode_Performance(xSystemHandle.BMP581_Handle) != HAL_OK) {
                xSystemFaultFlags.Flags |= BMP581_MODE_PERFORMANCE_FAILED;
            }
            if (IIM42653_Mode_Performance(xSystemHandle.IIM42653_Handle) != HAL_OK) {
                xSystemFaultFlags.Flags |= IIM42653_MODE_PERFORMANCE_FAILED;
            }
            if (IIS2MDCTR_Mode_Performance(xSystemHandle.IIS2MDCTR_Handle) != HAL_OK) {
                xSystemFaultFlags.Flags |= IIS2MDCTR_MODE_PERFORMANCE_FAILED;
            }
            break;
        default:
            break;
    }
}

void vSensorTask(void *pvParameters) {
    SensorTaskContext_t *TaskContext = pvParameters;
    SystemHandle_t *Handles = TaskContext->Handles;
    SystemContext_t *xSystemContext = TaskContext->xSystemContext;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    SensorMode_t requested_mode;
    SensorMode_t active_mode = SENSOR_MODE_OFF;

    // ReSharper disable once CppDFAEndlessLoop
    for (;;) {
        if (xQueueReceive(xSensorModeQueue, &requested_mode, 0) == pdPASS) {
            if (requested_mode != active_mode) {
                vApplyMode(xSystemContext, requested_mode, *Handles);
                active_mode = requested_mode;
            }
        }

        SensorData_t CurrentData = {0};
        StateEvent_t CurrentDataEvent = {0};
#if HIL_MODE
        SensorPayload_t HILData = {0};
        if (xQueueReceive(xHILModeQueue, &HILData, 0) == pdPASS) {
            CurrentData.Sync = SD_LOG_SYNC;
            CurrentData.TimestampMS = HILData.timestamp_ms;
            CurrentData.AccelX = HILData.ax;
            CurrentData.AccelY = HILData.ay;
            CurrentData.AccelZ = HILData.az;
            CurrentData.GyroX = HILData.gx;
            CurrentData.GyroY = HILData.gy;
            CurrentData.GyroZ = HILData.gz;
            CurrentData.MagX = HILData.mx;
            CurrentData.MagY = HILData.my;
            CurrentData.MagZ = HILData.mz;
            CurrentData.PressurePa = HILData.press;
            CurrentData.TemperatureC = HILData.temp;
            CurrentData.Latitude = HILData.lat;
            CurrentData.Longitude = HILData.lon;

            CurrentData.Altitude = CalculateAltitude(CurrentData.PressurePa, CurrentData.TemperatureC, xSystemContext->ReferencePressurePa, xSystemContext->ReferencePressurePaValid);
            CurrentData.Altitude = CalculateFilteredAltitude(xSystemContext, CurrentData.Altitude);
            CurrentData.VelocityZ = CalculateVelocityZ(xSystemContext, Handles->TIM2_Handle, CurrentData.Altitude);

            CurrentDataEvent = CreateDataStateEvent(&CurrentData);
            dbg_sensor_task_counter++;
            xQueueSend(xStateEventQueue, &CurrentDataEvent, 0);
        }
#else
        // TODO: Read real sensors here
        // BMP280_SensorData_t BMP280_SensorData;
        // BMP280_ReadData(Handles->BMP280_Handle, &BMP280_SensorData);

        // CurrentData.press = BMP280_SensorData.pressure_pa;
        // CurrentData.temp = BMP280_SensorData.temperature_c;

        BMP581_SensorData_t BMP581_SensorData;
        IIS2MDCTR_SensorData_t IIS2MDCTR_SensorData;

        static IIM42653_SensorData_t IIM42653_SensorData;
        bool IIM42653_DataReady = false;

        BMP581_GetData(Handles->BMP581_Handle, &BMP581_SensorData);
        IIS2MDCTR_GetData(Handles->IIS2MDCTR_Handle, &IIS2MDCTR_SensorData);

        if (IIM42653_CheckDataReady(Handles->IIM42653_Handle, &IIM42653_DataReady) == HAL_OK) {
            if (IIM42653_DataReady) {
                IIM42653_GetData(Handles->IIM42653_Handle, &IIM42653_SensorData);
            }
        }

        CurrentData.Sync = SD_LOG_SYNC;
        CurrentData.PressurePa = BMP581_SensorData.PressurePa;
        CurrentData.TemperatureC = BMP581_SensorData.TemperatureC;
        CurrentData.MagX = IIS2MDCTR_SensorData.MagX;
        CurrentData.MagY = IIS2MDCTR_SensorData.MagY;
        CurrentData.MagZ = IIS2MDCTR_SensorData.MagZ;

        CurrentData.GyroX = CalculateBiasedGyroscope(xSystemContext, IIM42653_SensorData.GyroX, xSystemContext->GyroBiasX);
        CurrentData.GyroY = CalculateBiasedGyroscope(xSystemContext, IIM42653_SensorData.GyroY, xSystemContext->GyroBiasY);
        CurrentData.GyroZ = CalculateBiasedGyroscope(xSystemContext, IIM42653_SensorData.GyroZ, xSystemContext->GyroBiasZ);
        CurrentData.AccelX = CalculateBiasedAcceleration(xSystemContext, IIM42653_SensorData.AccelX, xSystemContext->AccelBiasX);
        CurrentData.AccelY = CalculateBiasedAcceleration(xSystemContext, IIM42653_SensorData.AccelY, xSystemContext->AccelBiasY);
        CurrentData.AccelZ = CalculateBiasedAcceleration(xSystemContext, IIM42653_SensorData.AccelZ, xSystemContext->AccelBiasZ);

        CurrentData.Altitude = CalculateAltitude(CurrentData.PressurePa, CurrentData.TemperatureC, xSystemContext->ReferencePressurePa, xSystemContext->ReferencePressurePaValid);
        CurrentData.Altitude = CalculateFilteredAltitude(xSystemContext, CurrentData.Altitude);
        CurrentData.VelocityZ = CalculateVelocityZ(xSystemContext, Handles->TIM2_Handle, CurrentData.Altitude);

        CurrentDataEvent = CreateDataStateEvent(&CurrentData);
        dbg_sensor_task_counter++;
        xQueueSend(xStateEventQueue, &CurrentDataEvent, 0);
#endif

        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
