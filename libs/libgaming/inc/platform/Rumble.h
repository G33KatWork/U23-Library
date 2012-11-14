#ifndef _RUMBLE_H_
#define _RUMBLE_H_

#include <stm32f4xx/stm32f4xx.h>

#define GPIO_RUMBLE_PIN				(GPIO_Pin_8)
#define GPIO_RUMBLE_PORT			(GPIOD)
#define GPIO_RUMBLE_CLK				RCC_AHB1Periph_GPIOD

void InitializeRumble(void);
void EnableRumble(void);
void DisableRumble(void);

#endif
