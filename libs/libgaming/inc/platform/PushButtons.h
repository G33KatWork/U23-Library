#ifndef _PUSHBUTTONS_H_
#define _PUSHBUTTONS_H_

#include <stdint.h>
#include <stm32f4xx/stm32f4xx.h>

#define GPIO_PUSHBUTTON_A_PIN		(GPIO_Pin_1)
#define GPIO_PUSHBUTTON_A_PORT		(GPIOB)
#define GPIO_PUSHBUTTON_A_CLK		(RCC_AHB1Periph_GPIOB)

#define GPIO_PUSHBUTTON_B_PIN		(GPIO_Pin_0)
#define GPIO_PUSHBUTTON_B_PORT		(GPIOB)
#define GPIO_PUSHBUTTON_B_CLK		(RCC_AHB1Periph_GPIOB)

#define GPIO_PUSHBUTTON_UP_PIN		(GPIO_Pin_7)
#define GPIO_PUSHBUTTON_UP_PORT		(GPIOB)
#define GPIO_PUSHBUTTON_UP_CLK		(RCC_AHB1Periph_GPIOB)

#define GPIO_PUSHBUTTON_DOWN_PIN	(GPIO_Pin_5)
#define GPIO_PUSHBUTTON_DOWN_PORT	(GPIOB)
#define GPIO_PUSHBUTTON_DOWN_CLK	(RCC_AHB1Periph_GPIOB)

#define GPIO_PUSHBUTTON_LEFT_PIN	(GPIO_Pin_8)
#define GPIO_PUSHBUTTON_LEFT_PORT	(GPIOB)
#define GPIO_PUSHBUTTON_LEFT_CLK	(RCC_AHB1Periph_GPIOB)

#define GPIO_PUSHBUTTON_RIGHT_PIN	(GPIO_Pin_4)
#define GPIO_PUSHBUTTON_RIGHT_PORT	(GPIOB)
#define GPIO_PUSHBUTTON_RIGHT_CLK	(RCC_AHB1Periph_GPIOB)

typedef struct {
	uint16_t Up       : 1;
	uint16_t Down     : 1;
	uint16_t Left     : 1;
	uint16_t Right    : 1;
	uint16_t A        : 1;
	uint16_t B        : 1;
	uint16_t User     : 1;
} pushbutton_button_state_t;

void InitializePushButtons();

pushbutton_button_state_t GetPushbuttonState();
pushbutton_button_state_t CurrentPushButtonState();

void HandlePushbuttonTimerIRQ();

#endif