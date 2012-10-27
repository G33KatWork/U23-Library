#include <platform/UserInterface.h>

static void InitializeTimer(void);

void InitializeUserInterface(void) {
	InitializeTimer();
	InitializePushButtons();
}

static void InitializeTimer(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	//Setup timer for statemachine
	//Enable the TIM2 gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
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

void TIM2_IRQHandler(void)
{
	HandleSnesTimerIRQ();
	HandlePushbuttonTimerIRQ();
}
