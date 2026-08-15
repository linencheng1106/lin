#include "CAN_IRQHandler.h"
#include "buzzer.h"
#include "led.h"
#include "Timer.h"
#include "can_protocol.h"

CAN_RxHeaderTypeDef RxHeader; // CAN接收帧头
uint8_t RxData[8]; // CAN接收数据，最多8字节


extern uint8_t led_flow_state;
extern uint8_t buzzer_trigger_times;

volatile uint8_t can_breath_command_pending = 0U; // 新命令标志：0没有，1有
volatile uint8_t can_breath_enable_request = CAN_BREATH_DISABLE; // 开关：0关闭，1开启
volatile uint16_t can_breath_speed_request = CAN_BREATH_SPEED_MIN; // 速度0x0001～0x1000

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(hcan->Instance == CAN1)
    {
        if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            if(RxHeader.StdId== 0x123)
            {
                buzzer_trigger_times = RxData[CAN_BUZZER_COUNT_INDEX];
            }
            
            else if(RxHeader.StdId == 0x001)
            {
                uint8_t breath_enable; // Data[0]：开关
                uint16_t breath_speed; // Data[1..2]：速度
                breath_enable = RxData[CAN_BREATH_ENABLE_INDEX];
                
                /* 高字节左移8位 */
                breath_speed = ((uint16_t)RxData[CAN_BREATH_SPEED_HIGH_INDEX] << 8U) |((uint16_t)RxData[CAN_BREATH_SPEED_LOW_INDEX]);
                
                /*
                * 先保存数据，最后设置pending标志。
                * LED任务看到pending=1时，数据已经准备完成。
                */
                can_breath_enable_request = breath_enable;
                can_breath_speed_request = breath_speed;

                can_breath_command_pending = 1U;
            }
        }
    }
    else if (hcan->Instance == CAN2)
    {
        if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {

        }
    }
}


