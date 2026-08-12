#ifndef LED_H
#define LED_H

#include "main.h"

/*
 * LED1/LED2: PA4/PA5, ordinary GPIO outputs.
 * LED3/LED4: PA6/PA7, TIM3 CH1/CH2 PWM outputs.
 */
#define LED_ON(x)     HAL_GPIO_WritePin(LED_##x##_GPIO_Port, LED_##x##_Pin, GPIO_PIN_SET)
#define LED_OFF(x)    HAL_GPIO_WritePin(LED_##x##_GPIO_Port, LED_##x##_Pin, GPIO_PIN_RESET)
#define LED_TOGGLE(x) HAL_GPIO_TogglePin(LED_##x##_GPIO_Port, LED_##x##_Pin)

void Led_Init(void);
void Led_AllOff(void);

/* Call Start once when entering a state, then call Task repeatedly. */
void Led_WaterStart(void);
void Led_WaterTask(void);

void Led_BreathStart(void);
void Led_BreathTask(void);

#endif
