#ifndef _LEDS_H_
#define _LEDS_H_

/*! @header LED.h
    @discussion This header is used define basic LED functionality
 */


#include <stm32f4xx/stm32f4xx.h>

/*!
	@function InitializeLEDs
	Initializes all 4 LEDs located around the MEMS Accelerometer and beneath the SNES Controller Ports.
	@discussion This function is called in Main.c 
 */
void InitializeLEDs(void);

/*!
	@function SetLEDs
	Toggles LEDS
	@param leds Integer Bitmask that toggles one or more LEDs
	@discussion sets LED2 and LED4
	<tt>
	@textblock
	SetLEDs((1<<1)|(1<<3));
	@/textblock
	</tt>
 */
static inline void SetLEDs(int leds)
{
	uint16_t val=GPIOD->ODR;
	val&=~(0x0f<<12);
	val|=leds<<12;
	GPIOD->ODR=val;
}

#endif
