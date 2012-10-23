#ifndef _RANDOM_H_
#define _RANDOM_H_

/*! @header NewlibSyscalls.h
    @discussion This header is used implement the hardware RNG (Random Number Generator)
 */

#include <stdint.h>

/*!
	@function InitializeRandom
	Initializes the RNG
	@discussion This function is called in Main.c
 */
void InitializeRandom(void);

/*!
	@function GetRandomInteger
	@return returns an random integer
	@discussion returns 0 if not initialized
	@discussion blocking function: loops while RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET
 */
uint32_t GetRandomInteger(void);

/*!
	@function DeInitializeRandom
	DeInitializes the hardware RNG
 */
void DeInitializeRandom(void);

#endif
