#ifndef CAN_IRQHANDLER_H
#define CAN_IRQHANDLER_H

#include "main.h"
#include "can.h"
#include "EXTI_IRQHandler.h"
#include "CAN_State.h"

HAL_StatusTypeDef CAN_SendBeepReply(void);
HAL_StatusTypeDef CAN_SendLedReply(uint8_t led_state);


#endif // CAN_IRQHANDLER_H
