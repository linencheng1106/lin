#include "stm32f4xx_hal.h"
#include "led.h"
#include "Timer.h"



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

static timer_struct flow_timer;
static uint8_t flow_led_on = 1; 

// void void flowing_led_init(void)
// {
//     led_off(1);led_off(2);led_off(3);led_off(4);
//     flow_led_on = 1;
//     led_flow_state = 0;
//     timer_get_time(&flow_timer,led_flow_delay);
// }

static timer_struct led_flowing_timer;
uint8_t led_flow_state = 0;
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
                    if(current_led>4)
                    {
                        current_led = 1;
                    }
                }
            }break;
        }
    }
    else if(led_flow_state==0)
    {
        led_off(1);led_off(2);led_off(3);led_off(4);
    }
}


timer_struct breathing_timer;
static uint32_t bright_time = 0;//每段时间片内led亮的时间
static BREATH_STATE breath_state = BREATH_READY_ON;

void breathing_led_init(void)//呼吸灯初始化
{
    led_off(3);
    led_off(4);
    bright_time = 0;
    breath_state = BREATH_READY_ON;
    timer_get_time(&breathing_timer, led_breathing_delay);
}

void breathing_led(void)//这一段逻辑跟按键的差不多，只是多了个READY_ON模式前判断是否灭完灯的if
{
    switch(breath_state)
    {
        case BREATH_READY_ON:
        {
            if(timer_inquery(&breathing_timer))
            {
                breath_state=BREATH_ON;
                led_on(3);led_on(4);
                timer_get_time(&breathing_timer,bright_time);
            }
        }break;     
        case BREATH_ON:
        {
            if(timer_inquery(&breathing_timer))
            {
                led_off(3);led_off(4);
                breath_state=BREATH_READY_ON;
                bright_time++;
                timer_get_time(&breathing_timer,led_breathing_delay-bright_time);
            }
            if(bright_time==20)
            {
                breath_state=BREATH_READY_OFF;
            }
        }break;
        case BREATH_READY_OFF:
        {
            if(timer_inquery(&breathing_timer))
            {
                breath_state=BREATH_OFF;
                led_on(3);led_on(4);
                timer_get_time(&breathing_timer,bright_time);
            }
        }break;
        case BREATH_OFF:
        {
            if(timer_inquery(&breathing_timer))
            {
                led_off(3);led_off(4);
                breath_state=BREATH_READY_OFF;
                bright_time--;
                timer_get_time(&breathing_timer,led_breathing_delay-bright_time);
            }
            if(bright_time==0)
            {
                breath_state=BREATH_READY_ON;
            }
        }break;
    }
}
