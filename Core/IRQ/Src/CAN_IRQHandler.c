#include "CAN_IRQHandler.h"
#include "Led.h"
#include "Beep.h"
#include "CAN_State.h"

CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

static volatile uint8_t led_state = 0U;


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    /* 处理 CAN1 接收的数据 */
    if (hcan->Instance == CAN1)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            //判断是不是扩展帧、数据帧、蜂鸣器id、1字节
            if ((RxHeader.IDE == CAN_ID_EXT) &&
                (RxHeader.RTR == CAN_RTR_DATA) &&
                (RxHeader.ExtId == 0x01020101U) &&
                (RxHeader.DLC == 1U))
            {
                CAN_State_SetBeep(RxData[0]);
            }

            //第二种模式判断
            else if ((RxHeader.IDE == CAN_ID_EXT) &&
                     (RxHeader.RTR == CAN_RTR_DATA) &&
                     (RxHeader.ExtId == 0x01020201U) &&
                     (RxHeader.DLC == 1U))
            {
                if (RxData[0] == 1U)
                {
                    can_current_state = CAN_STATE_WATER;
                    led_state = 1U;
                }
                
                else if (RxData[0] == 0U)
                {
                    can_current_state = CAN_STATE_IDLE;
                    led_state = 0U;
                }
                else
                {
                    return;
                }

                CAN_SendLedReply(led_state);
            }
        }
    }
}


HAL_StatusTypeDef CAN_SendBeepReply(void)
{
    CAN_TxHeaderTypeDef TxHeader = {0};
    uint32_t TxMailbox;
    uint8_t TxData[2] = {'O', 'K'};

    TxHeader.ExtId = 0x02010101U;
    TxHeader.IDE = CAN_ID_EXT;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.DLC = 2U;
    TxHeader.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(&hcan1,&TxHeader,TxData,&TxMailbox);
}

HAL_StatusTypeDef CAN_SendLedReply(uint8_t led_state)
{
    CAN_TxHeaderTypeDef TxHeader = {0};
    uint32_t TxMailbox;
    uint8_t TxData[3] = {'O', 'K', led_state};

    TxHeader.ExtId = 0x02010201U;
    TxHeader.IDE = CAN_ID_EXT;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.DLC = 3U;
    TxHeader.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(&hcan1,&TxHeader,TxData,&TxMailbox);
}
