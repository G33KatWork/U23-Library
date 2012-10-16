#include <game/Game.h>

#include <stdio.h>
#include <string.h>

void Red_Init(struct Gamestate*);
void Red_Update(uint32_t);
void Red_Draw(Bitmap* surface);

Gamestate RedState = { 0, Red_Init, NULL, NULL, Red_Update, Red_Draw, NULL };

void Red_Init(struct Gamestate* state)
{
	printf("Second State\r\n");
}

void Red_Update(uint32_t delta)
{
	snes_button_state_t state = GetControllerState();
	if (state.buttons.Down)
		ExitState(TheGame);
}

void Red_Draw(Bitmap* surface)
{
	ClearBitmap(surface);
	DrawFilledRectangle(surface, 30, 30, 50, 50, RGB(255,0,0));
}
