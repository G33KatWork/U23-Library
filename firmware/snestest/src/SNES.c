#include <SNES.h>
#include <stm32f4xx/stm32f4xx.h>

//TODO: copy code and do the same for a second controller

enum SNES_CTRL_STATE
{
	START_LATCH = 0, END_LATCH,
	SHIFT1, SHIFT1_END,
	SHIFT2, SHIFT2_END,
	SHIFT3, SHIFT3_END,
	SHIFT4, SHIFT4_END,
	SHIFT5, SHIFT5_END,
	SHIFT6, SHIFT6_END,
	SHIFT7, SHIFT7_END,
	SHIFT8, SHIFT8_END,
	SHIFT9, SHIFT9_END,
	SHIFT10, SHIFT10_END,
	SHIFT11, SHIFT11_END,
	SHIFT12, SHIFT12_END,
	SHIFT13, SHIFT13_END,
	SHIFT14, SHIFT14_END,
	SHIFT15, SHIFT15_END,
	SHIFT16, SHIFT16_END
};

volatile enum SNES_CTRL_STATE state_ctrl1 = START_LATCH;
snes_button_state_t buttons_ctrl1 = {.raw = 0};
volatile uint16_t buttons_tmp_ctrl1 = 0;

void InitializeSnesController()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//Enable GPIOD clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	//Configure pins
	//PD0 = Data (Input, Pullup)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//PD1 = Latch (Output)
	//PD3 = Clock (Output)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//Set clock
	GPIO_SetBits(GPIOD, GPIO_Pin_3);
	
	//Reset latch
	GPIO_ResetBits(GPIOD, GPIO_Pin_1);


	//Setup timer for statemachine
	//Enable the TIM2 gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//TIM2 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 500 - 1;  // 1 MHz down to 0.5 KHz (500 us)
	TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; // 24 MHz Clock down to 1 MHz (adjust per your clock)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//TIM IT enable
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//TIM2 enable counter
	TIM_Cmd(TIM2, ENABLE);
}

snes_button_state_t GetControllerState()
{
	return buttons_ctrl1;
}

void HandleSnesTimerIRQ()
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		switch(state_ctrl1) {
			case START_LATCH:
				GPIO_SetBits(GPIOD, GPIO_Pin_1);	//Latch = 1
				break;

			case END_LATCH:
				GPIO_ResetBits(GPIOD, GPIO_Pin_1);	//Latch = 0
				break;

			case SHIFT1:
			case SHIFT2:
			case SHIFT3:
			case SHIFT4:
			case SHIFT5:
			case SHIFT6:
			case SHIFT7:
			case SHIFT8:
			case SHIFT9:
			case SHIFT10:
			case SHIFT11:
			case SHIFT12:
			case SHIFT13:
			case SHIFT14:
			case SHIFT15:
			case SHIFT16:
				GPIO_ResetBits(GPIOD, GPIO_Pin_3);	//CLK = 0
				break;

			case SHIFT1_END:
			case SHIFT2_END:
			case SHIFT3_END:
			case SHIFT4_END:
			case SHIFT5_END:
			case SHIFT6_END:
			case SHIFT7_END:
			case SHIFT8_END:
			case SHIFT9_END:
			case SHIFT10_END:
			case SHIFT11_END:
			case SHIFT12_END:
			case SHIFT13_END:
			case SHIFT14_END:
			case SHIFT15_END:
			case SHIFT16_END:
				buttons_tmp_ctrl1 = (buttons_tmp_ctrl1 << 1) | GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0);
				GPIO_SetBits(GPIOD, GPIO_Pin_3);	//CLK = 1
				break;
		}

		if(state_ctrl1 == SHIFT16_END)
		{
			state_ctrl1 = START_LATCH;
			buttons_ctrl1.raw = ~buttons_tmp_ctrl1;
		}
		else
			state_ctrl1++;
	}
}
