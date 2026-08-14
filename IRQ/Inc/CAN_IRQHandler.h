#ifndef _CAN_IQRQHANDLER_H_
#define _CAN_IQRQHANDLER_H_

#include "main.h"
#include "can.h"


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void CAN_BuzzerTimeoutTask(void);

#endif
