#ifndef _LEDS_H_
#define _LEDS_H_

#include <stm32f4xx/stm32f4xx.h>

void InitializeLEDs(void);

static inline void SetLEDs(int leds)
{
	uint16_t val=GPIOD->ODR;
	val&=~(0x0f<<12);
	val|=leds<<12;
	GPIOD->ODR=val;
}

#endif
