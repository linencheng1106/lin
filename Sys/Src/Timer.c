#include "Timer.h"
#include "stm32f4xx_hal.h"

void timer_get_time(timer_struct *timer,uint32_t duration_ms)
{
    timer->timer_start = HAL_GetTick();
    timer->timer_duration_ms = duration_ms;
   
}

int timer_inquery(timer_struct *timer)
{
    return (HAL_GetTick()-timer->timer_start>=timer->timer_duration_ms);
}
