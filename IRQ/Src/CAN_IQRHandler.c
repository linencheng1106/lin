#include "CAN_IRQHandler.h"
#include "buzzer.h"
#include "led.h"
#include "Timer.h"

CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];
static timer_struct CAN_buzzer_timer;
static uint8_t buzzer_ack_pending = 0;
extern uint8_t led_flow_state;
extern uint8_t buzzer_trigger_times;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if(hcan->Instance == CAN1)
    {
        if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            if(RxHeader.ExtId== 0x01020101)
            {
                buzzer_trigger_times = RxData[0];
                uint32_t buzzer_time = buzzer_trigger_times*200-100;
                timer_get_time(&CAN_buzzer_timer,buzzer_time);
                buzzer_ack_pending = 1;
            }
            else if(RxHeader.ExtId == 0x01020201)
            {

                led_flow_state=RxData[0];
                

                char TxData[3]={'O','K'};
				CAN_TxHeaderTypeDef TxHeader;
                uint32_t TxMailbox;
                TxData[2]=led_flow_state;
                TxHeader.StdId = 0x102;
                TxHeader.ExtId = 0x02010201;
                TxHeader.RTR = CAN_RTR_DATA;
                TxHeader.IDE = CAN_ID_EXT;
                TxHeader.DLC = 3;
                TxHeader.TransmitGlobalTime = DISABLE;
                HAL_CAN_AddTxMessage(hcan, &TxHeader, (uint8_t*)TxData, &TxMailbox);
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

void CAN_BuzzerTimeoutTask(void)
{
    if(buzzer_ack_pending && timer_inquery(&CAN_buzzer_timer))
    {
        buzzer_ack_pending = 0;
        CAN_TxHeaderTypeDef TxHeader;
        uint32_t TxMailbox;
        char TxData[2]={'O','K'};
        TxHeader.StdId = 0x102;
        TxHeader.ExtId = 0x02010101;
        TxHeader.RTR = CAN_RTR_DATA;
        TxHeader.IDE = CAN_ID_EXT;
        TxHeader.DLC = 2;
        TxHeader.TransmitGlobalTime = DISABLE;
        HAL_CAN_AddTxMessage(&hcan1, &TxHeader, (uint8_t*)TxData, &TxMailbox);
    }
}
