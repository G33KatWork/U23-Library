#include <game/Game.h>

#include <stdio.h>
#include <string.h>

void Red_Init(struct Gamestate*);
extern void OnEnter(struct Gamestate*);
extern void OnLeave(struct Gamestate*);
void Red_Update(uint32_t);
void Red_Draw(Bitmap* surface);

Gamestate RedState = { Red_Init, OnEnter, OnLeave, Red_Update, Red_Draw };

void Red_Init(struct Gamestate* state)
{
	printf("Second State\r\n");
}

void Red_Update(uint32_t delta)
{
	snes_button_state_t state = GetControllerState1();
	if (state.buttons.Down)
		ExitState();
}

void Red_Draw(Bitmap* surface)
{
	ClearBitmap(surface);
	DrawFilledRectangle(surface, 30, 30, 50, 50, RGB(255,0,0));
}
