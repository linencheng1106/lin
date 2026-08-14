#ifndef _UART_IRQHANDLER_H_
#define _UART_IRQHANDLER_H_

#include "main.h"
#include "usart.h"

void UART_Start_Receive(void);
void UART_Send_Sine_Start(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

extern uint8_t rx_buffer[5];

#endif 
