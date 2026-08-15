#ifndef _LED_H_
#define _LED_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

typedef enum{
    BREATH_READY_ON,
    BREATH_ON,
    BREATH_READY_OFF,
    BREATH_OFF,
}BREATH_STATE;

#define LED_GPIO_PORT GPIOA
#define LED1_PIN      GPIO_PIN_4
#define LED2_PIN      GPIO_PIN_5
#define LED3_PIN      GPIO_PIN_6
#define LED4_PIN      GPIO_PIN_7 

extern uint8_t led_flow_state;
#define led_breathing_delay  20//呼吸灯每份的时长

#define led_flow_period_on 300 
#define led_flow_period_off 30

void led_on(uint8_t led_num);
void led_off(uint8_t led_num);
void flowing_led_init(void);
void flowing_led(void);
void breathing_led_init(void);
void breathing_led(void);
void breathing_led_set(uint8_t enable, uint16_t speed);




#ifdef __cplusplus
}
#endif



#endif
