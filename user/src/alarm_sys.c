/* 写在前面：设计思路简要 */
/* mode 保存“现在是什么模式”
        ↓
   switch(mode) 选择对应行为
        ├─ STANDBY：LED 全灭、蜂鸣器不响
        ├─ RUN：LED 正常速度流水
        └─ ALARM：LED 快速流水、蜂鸣器响两次
        ↓
   修改 mode，下一次 alarm_sys_func() 自动进入下一模式
*/


#include "alarm_sys.h"
#include "led.h"
#include "buzzer.h"

#define LED_COUNT  4U
#define NORMAL_MS 300U  /* 运行模式的闪烁速度 */
#define ALARM_MS  100U  /* 报警模式的闪烁速度（加快）*/
#define BEEP_MS   100U  /* 每次蜂鸣持续时间 */
#define GAP_MS    200U  /* 两次蜂鸣的间隔 */

/* 工三种模式：待机、运行、报警 */
typedef enum { STANDBY, RUN, ALARM } Mode;
static Mode mode = STANDBY;  /* 当前模式，程序开始时待机 */

/* 从main搬运来的：指定 LED 闪烁指定次数 */
void blink_led(uint8_t led_num, uint16_t times, uint32_t delay_ms)
{
  uint16_t i = 0U; /* 循环计数变量 */

  /* if 判断：LED 编号只允许 1~4 */
  if (led_num > LED_COUNT)
  {
    return; /* return 直接结束当前函数 */
  }

  /* for 循环：初始化; 判断条件; 每次循环后执行 */
  for (i = 0U; i < times; i++)
  {
    led_on(led_num);          /* 点亮指定 LED */
    HAL_Delay(delay_ms);      /* 延时一段时间 */
    led_off(led_num);         /* 熄灭指定 LED */
    HAL_Delay(delay_ms);
  }
}

/* 从main搬运来的：蜂鸣器响一次 */
void beep(uint32_t beep_ms)
{
  buzzer_on();          /* 打开蜂鸣器 */
  HAL_Delay(beep_ms);   /* 保持响一段时间 */
  buzzer_off();         /* 关闭蜂鸣器 */
}

/* 报警系统主函数 */
void alarm_sys_func(void)
{
  uint8_t led;
  static uint8_t initialized = 0U; /* 确保初始化只执行一次 */

  if (initialized == 0U)           /* 第一次进入系统 */
  {
    buzzer_init();                 /* 蜂鸣器默认关闭 */
    initialized = 1U;              /* 标记：已经初始化 */
  }

  switch (mode)                    /* 根据当前模式执行不同功能 */
  {
    case STANDBY:
      for (led = 1U; led <= LED_COUNT; led++) led_off(led); /* 灯全灭 */
      HAL_Delay(1000U);            /* 待机 1 秒 */
      mode = RUN;                   /* 下一轮进入运行模式 */
      break;

    case RUN:
      for (led = 1U; led <= LED_COUNT; led++) /* LED1 到 LED4 */
        blink_led(led, 1U, NORMAL_MS);         /* 正常速度各闪一次 */
      mode = ALARM;                 /* 下一轮进入报警模式 */
      break;

    case ALARM:
      for (led = 1U; led <= LED_COUNT; led++) /* LED1 到 LED4 */
        blink_led(led, 1U, ALARM_MS);          /* 快速流水 */
      beep(BEEP_MS);                /* 第一次报警声 */
      HAL_Delay(GAP_MS);            /* 两次响声之间停顿 */
      beep(BEEP_MS);                /* 第二次报警声 */
      mode = STANDBY;               /* 下一轮回到待机 */
      break;

    default:
      mode = STANDBY;               /* 保护措施：意外情况处理 */
      break;
  }
}