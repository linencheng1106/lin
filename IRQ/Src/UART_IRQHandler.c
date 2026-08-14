#include "UART_IRQHandler.h"
#include "buzzer.h"
#include <math.h>
#include <string.h>

uint8_t rx_buffer[5]={0};


static uint8_t  tx_sine_buf[8];       
static float    tx_sine_phase = 0.0f;

static void pack_sine_frame(void)
{
    float val = sinf(tx_sine_phase);
    tx_sine_phase += 0.05f;
    if (tx_sine_phase > 6.2831853f)
    {
        tx_sine_phase -= 6.2831853f;
    }

    memcpy(&tx_sine_buf[0], &val, 4);    
    tx_sine_buf[4] = 0x00; tx_sine_buf[5] = 0x00;
    tx_sine_buf[6] = 0x80; tx_sine_buf[7] = 0x7F;
}

void UART_Send_Sine_Start(void)
{
    pack_sine_frame();
    HAL_UART_Transmit_DMA(&huart1, tx_sine_buf, 8);
}

/* DMA 发送完成中断回调：发完一帧立刻发下一帧，正弦波连续输出 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        pack_sine_frame();
        HAL_UART_Transmit_DMA(&huart1, tx_sine_buf, 8);
    }
}

void UART_Start_Receive(void)
{
    // HAL_UARTEx_ReceiveToIdle_IT(&huart1,rx_buffer,sizeof(rx_buffer)0);
    HAL_UART_Receive_DMA(&huart1,rx_buffer,5);
}

// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart1,uint16_t Size)
// {
//     if(huart1->Instance == USART1)
//     {
//         if(rx_buffer[0]==0xFF)
//         {
//             for(uint8_t i=1;i<Size;i++)
//             {
//                 if(rx_buffer[i]==1)
//                 {
//                     buzzer_trigger++;
//                 }
//             }
//             HAL_UART_Transmit(huart1,rx_buffer,Size,100);
//         }

//         HAL_UARTEx_ReceiveToIdle_IT(huart1,rx_buffer,sizeof(rx_buffer));
//     }
// }




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        if(rx_buffer[0]==0xFF)
        {
            for(uint8_t i=1;i<5;i++)
            {
                if(rx_buffer[i]==1)
                {
                    buzzer_trigger++;
                }
            }
        }

        HAL_UART_Receive_DMA(&huart1,rx_buffer,5);
    }
}


