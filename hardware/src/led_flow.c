#include "led_flow.h"
#include "led.h"

static const led_step_config led_flow_steps[LED_FLOW_STEP_COUNT] =
{
    { 0U, 100U, 100U },
    { 1U, 100U, 100U },
    { 2U, 100U, 100U },
    { 3U, 100U, 100U }
};

static void led_flow_step(led_step_config step)
{
    led_on(step.led_num);
    HAL_Delay(step.on_ms);
    led_off(step.led_num);
    HAL_Delay(step.off_ms);
}

void config_apply(led_flow_config config)
{
    for (uint8_t i = 0U; i < LED_FLOW_STEP_COUNT; i++)
    {
        led_flow_step(config.steps[i]);
    }
}