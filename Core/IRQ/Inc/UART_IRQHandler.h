#ifndef UART_IRQHANDLER_H
#define UART_IRQHANDLER_H

#include "main.h"
#include "usart.h"

void UART_Start_Recieve(void);
/* 发送一个float到VOFA+ */
HAL_StatusTypeDef UART_Send_VOFA_Float(float value);

#endif // UART_IRQHANDLER_H