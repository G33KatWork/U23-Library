#include <game/Game.h>
#include <game/Font.h>
#include "../inc/Highscore.h"

#include <stdio.h>
#include <string.h>

void Menu_Init(struct Gamestate*);
void Menu_OnEnter(struct Gamestate*);
void Menu_Update(uint32_t);
void Menu_Draw(Bitmap* surface);

void Highscore_Init(struct Gamestate*);
void Highscore_OnEnter(struct Gamestate*);
void Highscore_OnLeave(struct Gamestate*);
void Highscore_Update(uint32_t);
void Highscore_Draw(Bitmap* surface);

void About_Init(struct Gamestate*);
void About_Update(uint32_t);
void About_Draw(Bitmap* surface);

char * TitleForIndex(int index);

Gamestate MenuState = { Menu_Init, Menu_OnEnter, NULL, Menu_Update, Menu_Draw };
Gamestate HighscoreState = { Highscore_Init, Highscore_OnEnter, NULL, Highscore_Update, Highscore_Draw };
Gamestate AboutState = { About_Init, NULL, NULL, About_Update, About_Draw };

extern Gamestate game;
Game* TheGame = &(Game) {&MenuState};

int32_t selectedIndex = 0;
const int32_t numberOfIndexes = 3;
static int32_t lastMenuUpdate;
static char *highscore = NULL;


void Menu_Init(struct Gamestate* state)
{
	selectedIndex = 0;
	setFont(fontwhite16);
}

void Menu_OnEnter(struct Gamestate* state) {
	lastMenuUpdate = SysTickCounter;
}

void Menu_Update(uint32_t delta)
{

	snes_button_state_t state = GetControllerState1();
	if (SysTickCounter-lastMenuUpdate > 30)
		{
		if (state.buttons.Up) {
			lastMenuUpdate = SysTickCounter;
			selectedIndex += 3;
			selectedIndex -= 1;
		} else if (state.buttons.Down) {
			lastMenuUpdate = SysTickCounter;
			selectedIndex += 1;
		}
	}

	selectedIndex = selectedIndex%numberOfIndexes;

	if (state.buttons.Start || state.buttons.A)
	{
		switch(selectedIndex) {
			case 0:
				ChangeState(&game);
				break;

			case 1:
				ChangeState(&HighscoreState);
				break;

			default:
				ChangeState(&AboutState);
				break;
		}
	}
}

void Menu_Draw(Bitmap* surface)
{
	ClearBitmap(surface);

	setFont(fontwhite16);
	
	int y_rowheight = 20;
	int y_offset=30;

	for (int i = 0; i < numberOfIndexes; ++i)
	{
		int y_pos = y_offset+y_rowheight*i;
		DrawText(surface, TitleForIndex(i) , 30, y_pos);
		if (selectedIndex == i)
		{
			DrawFilledCircle(surface, 10, y_pos+6, 5, RGB(255, 0, 0));
		}
	}
}

char * TitleForIndex(int index) {
	switch(index) {
		case 0:
			return "Start";	
		case 1:
			return "Highscore";
		case 2:
			return "About";
		default:
			return NULL;
	}
}

void Highscore_Init(struct Gamestate* state) {}


void Highscore_OnEnter(struct Gamestate* state) {
	GetHighscoreList(&highscore);
}

void Highscore_OnLeave(struct Gamestate* state) {
	// if (highscore) {
	// 	free(highscore);
	// }
}

void Highscore_Update(uint32_t delta)
{
	snes_button_state_t state = GetControllerState1();
	if (SysTickCounter-lastMenuUpdate > 30) {
		if (state.raw > 0) {
			lastMenuUpdate = SysTickCounter;
			ExitState();
		}
	}

}

void Highscore_Draw(Bitmap* surface)
{
	ClearBitmap(surface);

	setFont(fontwhite16);
	if (highscore) {
		DrawText(surface, highscore , 10, 10);
	} else {
		DrawFilledCircle(surface, 100, 100, 10, RGB(0, 0, 255));
	}
}

void About_Init(struct Gamestate* state) {}

void About_Update(uint32_t delta)
{
	snes_button_state_t state = GetControllerState1();
	if (SysTickCounter-lastMenuUpdate > 30) {
		if (state.raw > 0) {
			lastMenuUpdate = SysTickCounter;
			ExitState();
		}
	}
}

void About_Draw(Bitmap* surface)
{
	ClearBitmap(surface);

	setFont(fontwhite16);
	DrawText(surface, "U23 2012\nspacecraft\n\npush any button\nto exit" , 10, 10);
}

