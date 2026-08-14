#include "Led.h"
#include "tim.h"

#define WATER_STEP_TIME_MS   300U
#define BREATH_STEP_TIME_MS  2U
#define BREATH_PWM_MAX       999U

static uint32_t water_last_tick;/*上次切换led的时刻*/
static uint8_t water_step;

static uint32_t breath_last_tick;
static uint16_t breath_pwm;
static int8_t breath_direction;

void Led_Init(void)
{
    if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    Led_AllOff();
}

void Led_AllOff(void)
{
    LED_OFF(1);
    LED_OFF(2);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0U);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0U);
}

void Led_WaterStart(void)
{
    Led_AllOff();

    water_step = 0U;
    water_last_tick = HAL_GetTick();

    LED_ON(1);
}

void Led_WaterTask(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - water_last_tick) < WATER_STEP_TIME_MS)
    {
        return;
    }

    water_last_tick = now;
    water_step ^= 1U;

    if (water_step == 0U)
    {
        LED_ON(1);
        LED_OFF(2);
    }
    else
    {
        LED_OFF(1);
        LED_ON(2);
    }
}

//----------------------------------------呼吸灯---------------------------------------//
void Led_BreathStart(void)
{
    Led_AllOff();

    breath_pwm = 0U;
    breath_direction = 1;
    breath_last_tick = HAL_GetTick();
}

void Led_BreathTask(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - breath_last_tick) < BREATH_STEP_TIME_MS)
    {
        return;
    }

    breath_last_tick = now;

    if (breath_direction > 0)
    {
        if (breath_pwm < BREATH_PWM_MAX)
        {
            breath_pwm++;
        }
        else
        {
            breath_direction = -1;
        }
    }
    else
    {
        if (breath_pwm > 0U)
        {
            breath_pwm--;
        }
        else
        {
            breath_direction = 1;
        }
    }

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, breath_pwm);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, breath_pwm);
}
