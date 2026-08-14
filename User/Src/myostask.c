#include "myostask.h"   

volatile uint32_t ledTaskStackHighWaterMark = 0;
volatile uint32_t buzzerTaskStackHighWaterMark = 0;

void LED_TASK(void *argument)
{
    static const uint16_t led_seq[]={LED1_PIN,LED2_PIN,LED3_PIN,LED4_PIN};
    uint8_t i=0;
    TickType_t xLastWakeTime=xTaskGetTickCount();
    const TickType_t xFrequency=pdMS_TO_TICKS(250);

    for(;;)
    {
        led_off(LED1_PIN|LED2_PIN|LED3_PIN|LED4_PIN);
        led_on(led_seq[i]);
        i=(i+1)%4;

        ledTaskStackHighWaterMark = uxTaskGetStackHighWaterMark(NULL)*sizeof(StackType_t);
        vTaskDelayUntil(&xLastWakeTime,xFrequency);
    }
}

void BUZZER_TASK_RTOS(void *argument)
{
    buzzer_init();
    for(;;)
    {
        buzzer_on();
        osDelay(100);
        buzzer_off();

        buzzerTaskStackHighWaterMark = uxTaskGetStackHighWaterMark(NULL)*sizeof(StackType_t);
        osDelay(100);
    }
}
