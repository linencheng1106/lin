#include "Beep.h"

#define BEEP_TIME_MS 100U

static uint8_t beep_active;
static uint32_t beep_start_tick;

void Beep_Init(void)
{
    beep_active = 0U;
    BEEP_OFF();
}

void Beep_Start(void)
{
    BEEP_ON();
    beep_start_tick = HAL_GetTick();
    beep_active = 1U;
}

void Beep_Task(void)
{
    if ((beep_active != 0U) &&
        ((HAL_GetTick() - beep_start_tick) >= BEEP_TIME_MS))
    {
        BEEP_OFF();
        beep_active = 0U;
    }
}
