#include "Key.h"

/* Key_Scan() is called every 10 ms. */
#define KEY_DEBOUNCE_COUNT   5U
#define KEY_LONG_COUNT       100U

typedef enum
{
    KEY_STATE_RELEASED = 0,
    KEY_STATE_PRESSED
} Key_StateTypeDef;

static Key_StateTypeDef key_state;
static GPIO_PinState last_raw_level; /*上一次的原始电平 */
static uint8_t debounce_count;/*读取到相同电平的次数*/
static uint16_t press_count;/*按键按下时间计数*/
static uint8_t long_event_sent;/*是否已经发送过长按事件*/
static volatile Key_TypeDef key_event;

void Key_Init(void)
{
    key_state = KEY_STATE_RELEASED;
    last_raw_level = HAL_GPIO_ReadPin(INPUT_1_GPIO_Port, INPUT_1_Pin);
    debounce_count = 0U;
    press_count = 0U;
    long_event_sent = 0U;
    key_event = KEY_NONE;
}

void Key_Scan(void)
{
    GPIO_PinState raw_level = HAL_GPIO_ReadPin(INPUT_1_GPIO_Port, INPUT_1_Pin);

    if (raw_level == last_raw_level)
    {
        if (debounce_count < KEY_DEBOUNCE_COUNT)
        {
            debounce_count++;
        }
    }
    else
    {
        last_raw_level = raw_level;
        debounce_count = 1U;
    }

    if (debounce_count >= KEY_DEBOUNCE_COUNT)
    {
        if ((key_state == KEY_STATE_RELEASED) && (raw_level == GPIO_PIN_RESET))
        {
            key_state = KEY_STATE_PRESSED;
            press_count = 0U;
            long_event_sent = 0U;
        }
        else if ((key_state == KEY_STATE_PRESSED) && (raw_level == GPIO_PIN_SET))
        {
            key_state = KEY_STATE_RELEASED;

            if (long_event_sent == 0U)
            {
                key_event = KEY_SHORT_PRESS;
            }

            press_count = 0U;
            long_event_sent = 0U;
        }
    }

    if (key_state == KEY_STATE_PRESSED)
    {
        if (press_count < KEY_LONG_COUNT)
        {
            press_count++;
        }

        if ((press_count >= KEY_LONG_COUNT) &&
            (long_event_sent == 0U))
        {
            long_event_sent = 1U;
            key_event = KEY_LONG_PRESS;
        }
    }
}

Key_TypeDef Key_GetEvent(void)
{
    Key_TypeDef event;

    event = key_event;
    key_event = KEY_NONE;


    return event;
}
