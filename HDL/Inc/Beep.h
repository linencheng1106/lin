#ifndef BEEP_H
#define BEEP_H

#include "main.h"

#define BEEP_ON()  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)

void Beep_Init(void);
void Beep_Start(void);
void Beep_Task(void);

#endif
