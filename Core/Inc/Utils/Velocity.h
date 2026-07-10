#ifndef VELOCITY_H
#define VELOCITY_H

#include "shared.h"

#define VELOCITY_Z_WINDOW_SIZE 10u

void ResetVelocityZWindow(void);
float CalculateVelocityZ(SystemContext_t *SystemContext, TIM_HandleTypeDef *TIM2_Handle, float CurrentAltitude);

#endif //VELOCITY_H
