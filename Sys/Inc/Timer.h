#ifndef _TIMER_H
#define _TIMER_H

#include "stm32f4xx_hal.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint32_t  timer_start;
    uint32_t  timer_duration_ms;
} timer_struct;

void timer_get_time(timer_struct *imer,uint32_t duration_ms);
int timer_inquery(timer_struct *timer);



#ifdef __cplusplus
}
#endif

#endif
