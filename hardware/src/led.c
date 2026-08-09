/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"
static const uint16_t led_pin[LED_NUM_MAX] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};
/* 点亮 LED1 */
void led_on(uint8_t led_num)
{
    if(LED_RIGHT_NUM(led_num))
    {
        return;
    }
    HAL_GPIO_WritePin(LED_GPIO_PORT, led_pin[led_num], GPIO_PIN_SET);
}

/* 熄灭 LED1 */
void led_off(uint8_t led_num)
{
    if(LED_RIGHT_NUM(led_num))
    {
        return;
    }
    HAL_GPIO_WritePin(LED_GPIO_PORT, led_pin[led_num], GPIO_PIN_RESET);
}

