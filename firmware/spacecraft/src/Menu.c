#include <game/Game.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>

void Menu_Init(struct Gamestate*);
void Menu_Update(uint32_t);
void Menu_Draw(Bitmap* surface);
char * TitleForIndex(int index);

Gamestate MenuState = { Menu_Init, NULL, NULL, Menu_Update, Menu_Draw };
extern Gamestate game;
Game* TheGame = &(Game) {&MenuState};

int32_t selectedIndex = 0;
const int32_t numberOfIndexes = 3;
static int32_t lastMenuUpdate;

void Menu_Init(struct Gamestate* state)
{
	selectedIndex = 0;
	setFont(fontwhite16);
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

			default:
				break;
		}
	}
}

void Menu_Draw(Bitmap* surface)
{
	ClearBitmap(surface);

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