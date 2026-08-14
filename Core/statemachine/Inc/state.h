#ifndef STATE_H
#define STATE_H

#include "main.h"


typedef enum
{
    STATE_OFF = 0,
    STATE_BEEP,
    STATE_WATER
} State_TypeDef;

void State_Init(void);
void State_Task(void);

State_TypeDef State_GetCurrent(void);

#endif
