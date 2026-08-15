/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "buzzer.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "CAN_IRQHandler.h"
#include "can.h"
#include "can_protocol.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t canTxTaskHandle;

const osThreadAttr_t canTxTask_attributes = {
  .name = "canTxTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* USER CODE END Variables */
/* Definitions for LEDTask */
osThreadId_t LEDTaskHandle;
const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for buzzerTask */
osThreadId_t buzzerTaskHandle;
uint32_t buzzerTaskBuffer[ 128 ];
osStaticThreadDef_t buzzerTaskControlBlock;
const osThreadAttr_t buzzerTask_attributes = {
  .name = "buzzerTask",
  .cb_mem = &buzzerTaskControlBlock,
  .cb_size = sizeof(buzzerTaskControlBlock),
  .stack_mem = &buzzerTaskBuffer[0],
  .stack_size = sizeof(buzzerTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void CAN_TX_TASK(void *argument);
/* USER CODE END FunctionPrototypes */
void LED_Task(void *argument);

void BUZZER_TASK(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LEDTask */
  LEDTaskHandle = osThreadNew(LED_Task, NULL, &LEDTask_attributes);

  /* creation of buzzerTask */
  buzzerTaskHandle = osThreadNew(BUZZER_TASK, NULL, &buzzerTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  canTxTaskHandle = osThreadNew(CAN_TX_TASK,NULL,&canTxTask_attributes);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}



/* USER CODE BEGIN Header_LED_Task */
/**
  * @brief  Function implementing the LEDTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_LED_Task */
void LED_Task(void *argument)
{
  /* USER CODE BEGIN LED_Task */
  /* Infinite loop */
  /*
  主板发送0x001
        ↓
  CAN接收回调解析开关和速度
        ↓
  can_breath_command_pending = 1
        ↓
  LED_Task读取新命令
        ↓
  breathing_led_set()
        ↓
  breathing_led()每1ms产生软件PWM
        ↓
  LED3、LED4呼吸
  */
  uint8_t enable; //0：关闭 1：开启
  uint16_t speed; //0x0001:最慢 0x1000:最快
  flowing_led_init();
  breathing_led_init();

  for (;;)
  {
    if (can_breath_command_pending != 0U)
      {
        taskENTER_CRITICAL();

        //不停检测can中断有没有把enable、speed变为全新状态，如有，要把can更改过的标志pending重新置为0
        enable = can_breath_enable_request; 
        speed = can_breath_speed_request;
        can_breath_command_pending = 0U;

        taskEXIT_CRITICAL();

        //更新呼吸灯参数开启呼吸灯
        breathing_led_set(enable, speed);
      }
    flowing_led();
    breathing_led();

    osDelay(1);
  }

  /* USER CODE END LED_Task */
}

/* USER CODE BEGIN Header_BUZZER_TASK */
/**
* @brief Function implementing the buzzerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BUZZER_TASK */
void BUZZER_TASK(void *argument)
{
  /* USER CODE BEGIN BUZZER_TASK */
  /* Infinite loop */
  buzzer_init();

  for (;;)
  {
    buzzer_alarm_fsm();
    osDelay(1);
  }
  /* USER CODE END BUZZER_TASK */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void CAN_TX_TASK(void *argument)
{
    uint32_t noise_sequence = 0U;
    uint8_t float_period_count = 0U;
    float feedback_value = 0.0f;

    TickType_t last_wake_time;
    last_wake_time = xTaskGetTickCount();

    for (;;)
    {
      /*
      * 每次循环都发送0x02010101。
      * 循环周期2 ms，因此频率为500 Hz。
      */
      if (CAN_SendSlaveNoise(noise_sequence) == HAL_OK)
      {
        noise_sequence++;
      }

      /*
       * 每次循环累计一次。
       * 5次 × 2 ms = 10 ms，即100 Hz。
      */
      float_period_count++;

      //500 Hz任务循环多少次之后，发送一次100 Hz float报文。
      if (float_period_count >= 5U)
      {
        float_period_count = 0U;

        if (CAN_SendFloatFeedback(feedback_value) == HAL_OK)
        {
          /*
           * 只有成功放入CAN发送邮箱后，
           * 才更新下一帧的float数值。
           */
          feedback_value += 0.1f;

          if (feedback_value > 10.0f)
          {
            feedback_value = 0.0f;
          }
        }
      }

        /*
         * 固定每2 ms唤醒一次，避免普通延时造成周期漂移。
         */
        vTaskDelayUntil(&last_wake_time,pdMS_TO_TICKS(CAN_NOISE_PERIOD_MS));
    }
}

/* USER CODE END Application */

