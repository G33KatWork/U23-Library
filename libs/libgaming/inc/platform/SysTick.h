#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include <stdint.h>

extern volatile uint32_t SysTickCounter;

void Delay(uint32_t time);

#endif
