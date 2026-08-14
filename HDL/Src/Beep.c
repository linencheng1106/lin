#include "Beep.h"

#define BEEP_ON_TIME_MS   100U
#define BEEP_OFF_TIME_MS  100U

static uint8_t beep_active;
static uint8_t beep_output_on;
static uint8_t beep_remaining;
static uint32_t beep_start_tick;

void Beep_Init(void)
{
    beep_active = 0U;
    beep_output_on = 0U;
    beep_remaining = 0U;

    BEEP_OFF();
}

/* 原来的一声蜂鸣功能 */
void Beep_Start(void)
{
    Beep_Can(1U);
}

/* 启动指定次数的蜂鸣 */
void Beep_Can(uint8_t count)
{
    if (count == 0U)
    {
        return;
    }

    beep_remaining = count;
    beep_active = 1U;
    beep_output_on = 1U;
    beep_start_tick = HAL_GetTick();

    BEEP_ON();
}

void Beep_Task(void)
{
    uint32_t now;

    if (beep_active == 0U)
    {
        return;
    }

    now = HAL_GetTick();

    if (beep_output_on != 0U)
    {
        /* 已经响了100ms */
        if ((now - beep_start_tick) >= BEEP_ON_TIME_MS)
        {
            BEEP_OFF();
            beep_output_on = 0U;
            beep_start_tick = now;

            if (beep_remaining > 0U)
            {
                beep_remaining--;
            }

            /* 所有次数已经完成 */
            if (beep_remaining == 0U)
            {
                beep_active = 0U;
            }
        }
    }
    else
    {
        /* 两声之间停止100ms */
        if ((now - beep_start_tick) >= BEEP_OFF_TIME_MS)
        {
            BEEP_ON();
            beep_output_on = 1U;
            beep_start_tick = now;
        }
    }
}

/* 返回1表示所有蜂鸣已经完成 */
uint8_t Beep_CanIsFinished(void)
{
    return (beep_active == 0U) ? 1U : 0U;
}