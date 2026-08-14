#ifndef _MYOSTASK_H_
#define _MYOSTASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"

#include "myostask.h"

#include "led.h"
#include "buzzer.h"

void LED_TASK(void *argument);
void BUZZER_TASK_RTOS(void *argument);

#endif

