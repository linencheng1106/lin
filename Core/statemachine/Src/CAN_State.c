#include "CAN_State.h"
#include "CAN_IRQHandler.h"
#include "Beep.h"
#include "Led.h"


volatile Can_State can_current_state = CAN_STATE_IDLE;
static volatile uint8_t can_beep_count = 0U;

static uint8_t beep_started = 0U;
static Can_State previous_state = CAN_STATE_IDLE;

void CAN_State_Init(void)
{
    can_current_state = CAN_STATE_IDLE;
    can_beep_count = 0U;
    beep_started = 0U;
    previous_state = CAN_STATE_IDLE;
}

void CAN_State_SetBeep(uint8_t count)
{
    if (count > 0U)
    {
        can_beep_count = count;
        can_current_state = CAN_STATE_BEEP;
    }
}

void CAN_State_Task(void)
{
    /* Run the entry action once whenever the state changes. */
    if (can_current_state != previous_state)
    {
        previous_state = can_current_state;

        switch (can_current_state)
        {
            case CAN_STATE_WATER:
                Led_WaterStart();
                break;

            case CAN_STATE_IDLE:
                Led_AllOff();
                break;

            case CAN_STATE_BEEP:
            default:
                break;
        }
    }

    switch (can_current_state)
    {
        case CAN_STATE_IDLE:
            beep_started = 0U;
            break;

        case CAN_STATE_BEEP:
            /* 进入状态后，只启动一次 */
            if (beep_started == 0U)
            {
                Beep_Can(can_beep_count);
                beep_started = 1U;
            }

            /* 所有次数已经响完 */
            if ((beep_started != 0U) &&(Beep_CanIsFinished() != 0U))
            {
                if (CAN_SendBeepReply() == HAL_OK)
                {
                    can_beep_count = 0U;
                    beep_started = 0U;
                    can_current_state = CAN_STATE_IDLE;
                }
            }
            break;

        case CAN_STATE_WATER:
            Led_WaterTask();
            break;

        default:
            can_current_state = CAN_STATE_IDLE;
            break;
    }

    /* 推进蜂鸣器响停过程 */
    Beep_Task();
}
