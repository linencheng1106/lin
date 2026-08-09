#include "led_flow.h"
#include "led.h"
extern int signal; // 声明外部变量 signal
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

/*MODE1、2、3分别为逐个亮灭、两两亮灭、一起亮灭*/
typedef enum
{
    LED_FLOW_MODE1 = 0,
    LED_FLOW_MODE2,
    LED_FLOW_MODE3
} led_flow_mode;

static void led_flow_mode1(void)
{
    void config_apply(led_flow_config config)
}

static void led_flow_mode2(void)
{
    led_on(0U);
    led_on(1U);
    HAL_Delay(100U);
    led_off(0U);
    led_off(1U);
    HAL_Delay(100U);

    led_on(2U);
    led_on(3U);
    HAL_Delay(100U);
    led_off(2U);
    led_off(3U);
    HAL_Delay(100U);
}

static void led_flow_mode3(void)
{
    led_on(0U);
    led_on(1U);
    led_on(2U);
    led_on(3U);
    HAL_Delay(100U);
    led_off(0U);
    led_off(1U);
    led_off(2U);
    led_off(3U);
    HAL_Delay(100U);
}

/*不同模式的选择*/
/*可以在main.c中手动更改模式*/
void led_flow_choose_mode(void)
{
  switch((led_flow_mode)signal)
  {
    case LED_FLOW_MODE1:
      led_flow_mode1();
      break;
    case LED_FLOW_MODE2:
      led_flow_mode2();
      break;
    case LED_FLOW_MODE3:
      led_flow_mode3();
      break;
    default:
      led_flow_mode1();
      break;
  }