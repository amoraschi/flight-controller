#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>
#include "stm32h7xx_hal.h"

static void Buzzer_Delay(uint32_t Duration, bool UseHAL) {
	if (UseHAL) {
		HAL_Delay(Duration);
	} else {
		vTaskDelay(pdMS_TO_TICKS(Duration));
	}
}

void Buzzer_Beep(uint32_t Duration) {
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
	Buzzer_Delay(Duration, false);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
}

void Buzzer_Beep_Counter(uint32_t BeepDuration, uint32_t BeepCount, uint32_t WaitDuration, bool UseHAL) {
	for (uint32_t Count = 0; Count < BeepCount; Count++) {
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
		Buzzer_Delay(BeepDuration, UseHAL);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_4);
		Buzzer_Delay(WaitDuration, UseHAL);
	}
}
