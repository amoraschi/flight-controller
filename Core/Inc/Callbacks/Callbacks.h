#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <timers.h>

void IIM42653_Timer_Callback(TimerHandle_t xTimer);
void BMP581_Timer_Callback(TimerHandle_t xTimer);
void IIS2MDCTR_Timer_Callback(TimerHandle_t xTimer);

#endif // CALLBACKS_H
