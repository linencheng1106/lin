#include "state.h"
#include "Led.h"
#include "Beep.h"
#include "Key.h"

static State_TypeDef current_state = STATE_OFF;

static void State_Change(State_TypeDef next_state)
{
    if (next_state == current_state)
    {
        return;
    }

    Led_AllOff();
    current_state = next_state;

    switch (current_state)
    {
        case STATE_WATER:
            Led_WaterStart();
            break;

        case STATE_BREATH:
            Led_BreathStart();
            break;

        case STATE_OFF:
        default:
            Led_AllOff();
            break;
    }

    Beep_Start();
}

void State_Init(void)
{
    current_state = STATE_OFF;
    Led_AllOff();
    Beep_Init();
}

void State_Task(void)
{
    Key_TypeDef key_event = Key_GetEvent();

    switch (current_state)
    {
        case STATE_OFF:
            if (key_event == KEY_LONG_PRESS)
            {
                State_Change(STATE_WATER);
            }
            break;

        case STATE_WATER:
            Led_WaterTask();

            if (key_event == KEY_SHORT_PRESS)
            {
                State_Change(STATE_BREATH);
            }
            break;

        case STATE_BREATH:
            Led_BreathTask();

            if (key_event == KEY_SHORT_PRESS)
            {
                State_Change(STATE_WATER);
            }
            break;

        default:
            current_state = STATE_OFF;
            Led_AllOff();
            break;
    }

    Beep_Task();
}

State_TypeDef State_GetCurrent(void)
{
    return current_state;
}
