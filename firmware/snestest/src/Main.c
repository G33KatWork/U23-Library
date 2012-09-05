#include <stdint.h>
#include <stdio.h>

#include <stm32f4xx/stm32f4xx.h>

#include <SystemInit.h>
#include <USART.h>
#include <LED.h>
#include <SNES.h>

#include <Main.h>

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	InitializeLEDs();
	MyUSART_Init();

	setvbuf(stdout, 0, _IONBF, 0);
	printf("Hello!\r\n");

	InitializeSnesController();

	while(1)
	{
		snes_button_state_t btn = GetControllerState();
		printf("SNES: %02X\r\n", btn.raw);
		printf(
			"%s %s %s %s %s %s %s %s %s %s %s %s\r\n",
			btn.buttons.A ? "A" : "",
			btn.buttons.B ? "B" : "",
			btn.buttons.X ? "X" : "",
			btn.buttons.Y ? "Y" : "",
			btn.buttons.L ? "L" : "",
			btn.buttons.R ? "R" : "",
			btn.buttons.Start ? "Start" : "",
			btn.buttons.Select ? "Select" : "",
			btn.buttons.Left ? "Left" : "",
			btn.buttons.Up ? "Up" : "",
			btn.buttons.Down ? "Down" : "",
			btn.buttons.Right ? "Right" : ""
		);

		SetLEDs(btn.buttons.A << 3 | btn.buttons.B << 2 | btn.buttons.X << 1 | btn.buttons.Y);
	}
}

volatile uint32_t SysTickCounter=0;

void Delay(uint32_t time)
{
	uint32_t end=SysTickCounter+time;
	while(SysTickCounter!=end);
}

void SysTick_Handler()
{  
	SysTickCounter++;
}

void TIM2_IRQHandler()
{
	HandleSnesTimerIRQ();
}
