#ifndef _SYSTICK_H_
#define _SYSTICK_H_

/*! @header SysTick.h
      @discussion SysTick is used to to implement a counter that is incremented all 10ms
 */

#include <stdint.h>

extern volatile uint32_t SysTickCounter;

/*! @function Delay
	Holds exceution for a given time.
	@param time Time to hold in 10ms steps
	@discussion Try to avoid this.
 */
void Delay(uint32_t time);

#endif
