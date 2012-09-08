#include <stdint.h>
#include <stdio.h>

#include <platform/SystemInit.h>
#include <platform/LED.h>
#include <platform/SNES.h>
#include <platform/SysTick.h>

#include <game/Game.h>

volatile uint32_t oldTime = 0;
volatile uint32_t currentTime = 0;

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	InitializeLEDs();
	InitializeSnesController();

	if(!TheGame) {
		fprintf(stderr, "PANIC: Game structure pointer is NULL\r\n");
		return -1;
	}

	if(TheGame->Init)
		TheGame->Init();

	if(!TheGame->Update || !TheGame->Draw) {
		fprintf(stderr, "PANIC: Update and/or Draw function pointer is NULL\r\n");
		return -1;
	}

	while(1)
	{
		oldTime = currentTime;
		currentTime = SysTickCounter;

		TheGame->Update(currentTime - oldTime);
		TheGame->Draw();

		//WaitForVBlank();
	}
}
