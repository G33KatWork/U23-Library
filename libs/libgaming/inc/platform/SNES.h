#ifndef _SNES_CTRL_H_
#define _SNES_CTRL_H_

#include <stdint.h>
#include <stm32f4xx/stm32f4xx.h>

#define GPIO_SNES1_LATCH_PIN		(GPIO_Pin_6)
#define GPIO_SNES1_CLOCK_PIN		(GPIO_Pin_7)
#define GPIO_SNES1_DATA_PIN			(GPIO_Pin_5)

#define GPIO_SNES2_LATCH_PIN		(GPIO_Pin_1)
#define GPIO_SNES2_CLOCK_PIN		(GPIO_Pin_3)
#define GPIO_SNES2_DATA_PIN			(GPIO_Pin_0)

#define GPIO_SNES1_LATCH_PORT		(GPIOD)
#define GPIO_SNES1_CLOCK_PORT		(GPIOD)
#define GPIO_SNES1_DATA_PORT		(GPIOD)

#define GPIO_SNES2_LATCH_PORT		(GPIOD)
#define GPIO_SNES2_CLOCK_PORT		(GPIOD)
#define GPIO_SNES2_DATA_PORT		(GPIOD)

#define GPIO_SNES1_LATCH_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES1_CLOCK_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES1_DATA_CLK			RCC_AHB1Periph_GPIOD

#define GPIO_SNES2_LATCH_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES2_CLOCK_CLK		RCC_AHB1Periph_GPIOD
#define GPIO_SNES2_DATA_CLK			RCC_AHB1Periph_GPIOD

typedef union {
	struct {
		uint16_t Reserved : 4;
		uint16_t R        : 1;
		uint16_t L        : 1;
		uint16_t X        : 1;
		uint16_t A        : 1;
		uint16_t Right    : 1;
		uint16_t Left     : 1;
		uint16_t Down     : 1;
		uint16_t Up       : 1;
		uint16_t Start    : 1;
		uint16_t Select   : 1;
		uint16_t Y        : 1;
		uint16_t B        : 1;
	} buttons;
	uint16_t raw;
} snes_button_state_t;

void InitializeSnesController();
snes_button_state_t GetControllerState1();
snes_button_state_t GetControllerState2();
void HandleSnesTimerIRQ();

#endif
