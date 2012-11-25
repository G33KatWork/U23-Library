#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <platform/LED.h>
#include <platform/UserInterface.h>
#include <platform/VGA.h>

#include <System.h>

#include <game/Filesystem.h>
#include <game/Game.h>

#include <Drawing.h>

uint32_t oldTime = 0;
uint32_t currentTime = 0;
uint32_t frame = 0;
Bitmap frame1, frame2;
Bitmap* drawingSurface;

#define ADDR_FRAMEBUFFER1	((uint8_t*)0x20000000)
#define ADDR_FRAMEBUFFER2	((uint8_t*)0x20010000)

int main()
{
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	InitializeLEDs();
	InitializeUserInterface();
	InitializeRandom();

	//Clear framebuffers
	memset(ADDR_FRAMEBUFFER1, 0x00, 320*200);
	memset(ADDR_FRAMEBUFFER2, 0x00, 320*200);

	//Create drawing surfaces
	InitializeBitmap(&frame1,320,200,320,ADDR_FRAMEBUFFER1);
	InitializeBitmap(&frame2,320,200,320,ADDR_FRAMEBUFFER2);

	//Switch on VGA
	IntializeVGAScreenMode320x200(ADDR_FRAMEBUFFER1);

	if(!TheGame) {
		EnableDebugOutput(DEBUG_USART);
		fprintf(stderr, "PANIC: Game structure pointer is NULL\r\n");
		return -1;
	}

	if(TheGame->currentState)
		TheGame->currentState->Init(NULL);

	if(!TheGame->currentState->Update || !TheGame->currentState->Draw) {
		fprintf(stderr, "PANIC: Update and/or Draw function pointer is NULL\r\n");
		return -1;
	}

	Gamestate *currentState = TheGame->currentState;

	while(1)
	{
		oldTime = currentTime;
		currentTime = SysTickCounter;
		if (currentState != TheGame->currentState)
		{
			Gamestate *newState = TheGame->currentState;
			if (!newState->initialized) {
				newState->Init(currentState);
				newState->initialized = true;
			}
			if (newState->OnEnter)
				newState->OnEnter(currentState);
			currentState = newState;
		}

		//Swap Buffers
		if(frame&1) { drawingSurface=&frame2; SetFrameBuffer(ADDR_FRAMEBUFFER1); }
		else { drawingSurface=&frame1; SetFrameBuffer(ADDR_FRAMEBUFFER2); }

		//Update and draw
		currentState->Update(currentTime - oldTime);
		currentState->Draw(drawingSurface);

		frame++;

		if (currentState != TheGame->currentState &&
			TheGame->currentState->OnLeave)
		{
			currentState->OnLeave(currentState);
		}
		//VSync
		WaitVBL();
	}
}
