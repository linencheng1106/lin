#include "TIM_IRQHandler.h"
#include "Key.h"

void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    
    if (htim->Instance == TIM2)
    {
        static uint8_t key_scan_div = 0;
        /*
         * 按键扫描
         */
        key_scan_div++;

        if (key_scan_div >= 5)
        {
            key_scan_div = 0;
            Key_Scan();
        }
    }
}
