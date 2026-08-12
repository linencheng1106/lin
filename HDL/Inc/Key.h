#ifndef KEY_H
#define KEY_H

#include "main.h"
typedef enum
{
    KEY_NONE = 0,
    KEY_SHORT_PRESS,
    KEY_LONG_PRESS,
} Key_TypeDef;

void Key_Init(void);
void Key_Scan(void);
Key_TypeDef Key_GetEvent(void);

#endif