#ifndef CAN_STATE_H
#define CAN_STATE_H

#include "main.h"

typedef enum
{
    CAN_STATE_IDLE = 0,
    CAN_STATE_BEEP,
    CAN_STATE_WATER
} Can_State;

/* Defined in CAN_State.c; read by the main loop and written by CAN IRQ. */
extern volatile Can_State can_current_state;

void CAN_State_Init(void);
void CAN_State_Task(void);

/* CAN接收回调通过这些接口传递命令 */
void CAN_State_SetBeep(uint8_t count);



#endif
