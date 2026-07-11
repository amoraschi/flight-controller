#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32h7xx_hal.h"

void BUZZER_Beep(uint32_t Duration) {
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
	vTaskDelay(pdMS_TO_TICKS(Duration));
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
}
