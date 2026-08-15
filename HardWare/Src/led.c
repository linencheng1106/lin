#include "stm32f4xx_hal.h"
#include "led.h"
#include "Timer.h"
#include "can_protocol.h"


void led_on(uint8_t led_num)
{
    switch (led_num)
    {
        case 1:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
            break;
        case 4:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}

void led_off(uint8_t led_num)
{
    switch (led_num)
    {
        case 1:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
            break;
        case 4:
            HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

static timer_struct led_flowing_timer;
uint8_t led_flow_state = 0U;

void flowing_led_init(void)
{
    led_off(1);
    led_off(2);

    led_flow_state = 1U;

    /* 让流水灯状态机立即启动 */
    timer_get_time(&led_flowing_timer, 0U);
}


void flowing_led()//流水灯函数
{
    if(led_flow_state==1)
    {
        static uint8_t current_led =1;
        typedef enum{
            LED_READY_ON,
            LED_ON,
            LED_OFF,
        }LED_FLOWING_STATE;
        static LED_FLOWING_STATE led_flowing_state = LED_READY_ON;
        

        switch(led_flowing_state)
        {
            case LED_READY_ON:
            {
                if(timer_inquery(&led_flowing_timer))
                {
                    led_flowing_state = LED_ON;
                }
            }break;
            case LED_ON:
            {
                    led_on(current_led);
                    led_flowing_state = LED_OFF;
                    timer_get_time(&led_flowing_timer,led_flow_period_on);
            }break;
            case LED_OFF:
            {
                if(timer_inquery(&led_flowing_timer))
                {
                    led_flowing_state = LED_READY_ON;
                    led_off(current_led);
                    timer_get_time(&led_flowing_timer,led_flow_period_off);
                    current_led++;
                    if(current_led>2)
                    {
                        current_led = 1;
                    }
                }
            }break;
        }
    }
    else if(led_flow_state==0)
    {
        led_off(1);led_off(2);
    }
}

//---------------------------------------------呼吸灯-----------------------------------------//
#define BREATH_PWM_PERIOD_MS       20U    // PWM周期20 ms
#define BREATH_CYCLE_SLOW_MS       4000U  // 最慢周期4000 ms
#define BREATH_CYCLE_FAST_MS       400U   // 最快周期400 ms

// 呼吸灯使用非阻塞设计
// 根据系统Tick计算当前呼吸周期相位，
// 前半周期渐亮，后半周期渐暗，
// 并使用20 ms软件PWM调节亮度。
// 顶层包含开启和关闭两种工作状态，
// 但亮度变化没有使用显式枚举状态机。//
static uint8_t breathing_enabled = 0U;// 呼吸灯开关：0关闭，1开启
static uint16_t breathing_speed = CAN_BREATH_SPEED_MIN; // 最小速度，对应4000 ms
static uint32_t breathing_cycle_period_ms = BREATH_CYCLE_SLOW_MS; // 当前呼吸周期，单位ms
static uint32_t breathing_start_tick = 0U; // 呼吸开始时间，单位ms

void breathing_led_init(void)
{
    breathing_enabled = 0U;
    breathing_speed = CAN_BREATH_SPEED_MIN;
    breathing_cycle_period_ms = BREATH_CYCLE_SLOW_MS;
    breathing_start_tick = HAL_GetTick();

    led_off(3);
    led_off(4);
}

void breathing_led_set(uint8_t enable, uint16_t speed)
{
    /* 防止速度数据超出协议范围 */
    if (speed < CAN_BREATH_SPEED_MIN)
    {
        speed = CAN_BREATH_SPEED_MIN;
    }
    else if (speed > CAN_BREATH_SPEED_MAX)
    {
        speed = CAN_BREATH_SPEED_MAX;
    }

    if (enable == CAN_BREATH_ENABLE)
    {
        breathing_enabled = 1U;
    }
    else
    {
        breathing_enabled = 0U;
    }

    breathing_speed = speed;

    /*
     * 将0x0001～0x1000转换为4000～400 ms。
     * speed越大，呼吸周期越短。
     * 按照比例来转化：(当前速度 - 最小速度) *(最慢周期 - 最快周期) / (最大速度 - 最小速度)
     * 从4000 ms中减去缩短量，速度越大，周期越短。
     */
    breathing_cycle_period_ms =
        BREATH_CYCLE_SLOW_MS -
        (((uint32_t)(breathing_speed - CAN_BREATH_SPEED_MIN) *
          (BREATH_CYCLE_SLOW_MS - BREATH_CYCLE_FAST_MS)) /
         (CAN_BREATH_SPEED_MAX - CAN_BREATH_SPEED_MIN));

    breathing_start_tick = HAL_GetTick();

    if (breathing_enabled == 0U)
    {
        led_off(3);
        led_off(4);
    }
}

//根据系统运行时间判断当前处于呼吸周期的什么位置。
//计算此刻应该有多亮。
//根据PWM决定这一毫秒LED应该亮还是灭。
void breathing_led(void)
{
    uint32_t elapsed; // 已运行时间，单位ms
    uint32_t cycle_position; // 当前呼吸周期位置
    uint32_t half_cycle; // 半周期：前半渐亮，后半渐暗
    uint32_t brightness; // 亮度等级0～20
    uint32_t pwm_position; // PWM位置0～19 ms

    if (breathing_enabled == 0U)
    {
        led_off(3);
        led_off(4);
        return;
    }

    //表示从本次呼吸开始到现在，已经过去了多少毫秒。
    elapsed = HAL_GetTick() - breathing_start_tick;

    cycle_position = elapsed % breathing_cycle_period_ms;
    half_cycle = breathing_cycle_period_ms / 2U;

    /*
     * 前半周期亮度由0增加到20；
     * 后半周期亮度由20降低到0。
     */
    
    /*假设完整呼吸周期为 1000 ms：
    在呼吸开始时：
    cycle_position = 0
    brightness = 0 × 20 ÷ 500 = 0
    经过 250 ms：
    brightness = 250 × 20 ÷ 500 = 10
    */
    if (cycle_position < half_cycle)
    {
        brightness = (cycle_position * BREATH_PWM_PERIOD_MS) / half_cycle;
    }
    
    /*假设完整周期为 1000 ms：
    在 500 ms 时：
    brightness = (1000 - 500) × 20 ÷ 500 = 20
    */
    else
    {
        brightness = ((breathing_cycle_period_ms - cycle_position) *BREATH_PWM_PERIOD_MS) / half_cycle;
    }

    pwm_position = elapsed % BREATH_PWM_PERIOD_MS; // 当前20 ms PWM位置
    
    //brightness 决定一个 20 ms 周期内，LED 点亮多少毫秒
    if (pwm_position < brightness)
    {
        led_on(3);
        led_on(4);
    }
    else
    {
        led_off(3);
        led_off(4);
    }
}
