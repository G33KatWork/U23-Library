#include <game/Game.h>
#include <game/Debug.h>
#include <stdio.h>

void Init(void);
void Update(uint32_t);
void Draw(void);

Game MyGame = { Init, Update, Draw };
Game* TheGame = &MyGame;

void Init()
{
	EnableDebugOutput(DEBUG_USART);
	printf("Init\r\n");
}

void Update(uint32_t delta)
{
	printf("Delta: %d\r\n", delta);
	Delay(100);
}

void Draw()
{

}
