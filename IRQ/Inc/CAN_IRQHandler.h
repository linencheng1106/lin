#ifndef CAN_IRQ_HANDLER_H
#define CAN_IRQ_HANDLER_H

#include "main.h"
#include "can.h"
#include <stdint.h>

/*
 * 已收到但尚未由FreeRTOS任务回复的0x012报文数量。
 * 中断中增加，后续CAN任务中减少。
 */


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);


extern volatile uint8_t can_breath_command_pending;
extern volatile uint8_t can_breath_enable_request;
extern volatile uint16_t can_breath_speed_request;

#endif
