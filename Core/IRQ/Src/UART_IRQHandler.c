#include "UART_IRQHandler.h"
#include "Beep.h"
#include <string.h>
#include "EXTI_IRQHandler.h"

uint8_t rx_buffer[100] = {0};
uint8_t tx_buffer[100] = {0};
uint8_t vofa_tx_buffer[8] = {0};

void UART_Start_Recieve(void)
{
    //HAL_UART_Receive_IT(&huart1, rx_buffer, 5);
    //HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
    //HAL_UART_Receive_DMA(&huart1, rx_buffer, 5);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    if (huart->Instance == USART1)
    {
        if(rx_buffer[0] == 0xFF)
        {
            for(uint8_t i = 1; i < 5; i++)
            {
                if(rx_buffer[i] == 1)
                {
                    Beep_Trigger++;
                }
            }
        }
        //HAL_UART_Receive_IT(&huart1, rx_buffer, 5);
        HAL_UART_Receive_DMA(&huart1, rx_buffer, 5);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        if(rx_buffer[0] == 0xAA)
        {
            for(uint8_t i = 1; i < Size; i++)
            {
                if(rx_buffer[i] == 1)
                {
                    Beep_Trigger++;
                }
            }
            memcpy(tx_buffer, rx_buffer, Size);
            HAL_UART_Transmit_IT(&huart1, tx_buffer, Size);
        }
        //HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
    }
}

HAL_StatusTypeDef UART_Send_VOFA_Float(float value)
{
    static const uint8_t vofa_tail[4] = {0x00, 0x00, 0x80, 0x7F};

    memcpy(&vofa_tx_buffer[0],&value,sizeof(value));
    memcpy(&vofa_tx_buffer[4],&vofa_tail,sizeof(vofa_tail));

    return HAL_UART_Transmit_IT(&huart1, vofa_tx_buffer, sizeof(vofa_tx_buffer));
}
