#include <platform/UserInterface.h>
#include <platform/PushButtons.h>

pushbutton_button_state_t CurrentPushButtonState();

pushbutton_button_state_t pushbutton_state = {0,0,0,0,0,0,0};
pushbutton_button_state_t pushbutton_state_debounce = {0,0,0,0,0,0,0};

void InitializePushButtons() 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(GPIO_PUSHBUTTON_A_CLK | GPIO_PUSHBUTTON_B_CLK | GPIO_PUSHBUTTON_DOWN_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(GPIO_PUSHBUTTON_LEFT_CLK | GPIO_PUSHBUTTON_RIGHT_CLK | GPIO_PUSHBUTTON_UP_CLK, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//Configure data pins (Input, Pullup)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_InitStructure.GPIO_Pin = GPIO_PUSHBUTTON_A_PIN;
	GPIO_Init(GPIO_PUSHBUTTON_A_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PUSHBUTTON_B_PIN;
	GPIO_Init(GPIO_PUSHBUTTON_B_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PUSHBUTTON_UP_PIN;
	GPIO_Init(GPIO_PUSHBUTTON_UP_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PUSHBUTTON_DOWN_PIN;
	GPIO_Init(GPIO_PUSHBUTTON_DOWN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PUSHBUTTON_LEFT_PIN;
	GPIO_Init(GPIO_PUSHBUTTON_LEFT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PUSHBUTTON_RIGHT_PIN;
	GPIO_Init(GPIO_PUSHBUTTON_RIGHT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// set initial state
	pushbutton_state = CurrentPushButtonState();
	pushbutton_state_debounce = pushbutton_state;
}

pushbutton_button_state_t GetPushbuttonState() {
	return pushbutton_state;
}

pushbutton_button_state_t CurrentPushButtonState() {
	pushbutton_button_state_t currentState;

	currentState.A = GPIO_ReadInputDataBit(GPIO_PUSHBUTTON_A_PORT, GPIO_PUSHBUTTON_A_PIN);
	currentState.B = GPIO_ReadInputDataBit(GPIO_PUSHBUTTON_B_PORT, GPIO_PUSHBUTTON_B_PIN);
	currentState.Up = GPIO_ReadInputDataBit(GPIO_PUSHBUTTON_UP_PORT, GPIO_PUSHBUTTON_UP_PIN);
	currentState.Down = GPIO_ReadInputDataBit(GPIO_PUSHBUTTON_DOWN_PORT, GPIO_PUSHBUTTON_DOWN_PIN);
	currentState.Left = GPIO_ReadInputDataBit(GPIO_PUSHBUTTON_LEFT_PORT, GPIO_PUSHBUTTON_LEFT_PIN);
	currentState.Right = GPIO_ReadInputDataBit(GPIO_PUSHBUTTON_RIGHT_PORT, GPIO_PUSHBUTTON_RIGHT_PIN);
	currentState.User = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

	return currentState;
}

void HandlePushbuttonTimerIRQ() {
	static int16_t delayCounter = 0;
	delayCounter++;
	if (delayCounter >= 3)
	{

		pushbutton_state.A &= pushbutton_state_debounce.A;
		pushbutton_state.B &= pushbutton_state_debounce.B;
		pushbutton_state.Up &= pushbutton_state_debounce.Up;
		pushbutton_state.Down &= pushbutton_state_debounce.Down;
		pushbutton_state.Left &= pushbutton_state_debounce.Left;
		pushbutton_state.Right &= pushbutton_state_debounce.Right;
		pushbutton_state.User &= pushbutton_state_debounce.User;

		delayCounter = 0;
		pushbutton_state = CurrentPushButtonState();	
	}
	pushbutton_state_debounce = CurrentPushButtonState();	
}
