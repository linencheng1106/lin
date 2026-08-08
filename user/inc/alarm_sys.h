#ifndef ALARM_SYS_H
#define ALARM_SYS_H

#include "stm32h7xx_hal.h"

void blink_led(uint8_t led, uint16_t times, uint32_t delay_ms);
void beep(uint32_t beep_ms);
void alarm_sys_func(void);

#endif