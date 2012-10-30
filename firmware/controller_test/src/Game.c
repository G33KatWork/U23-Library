#include <game/Game.h>
#include <game/Debug.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>

#include <ControllerBitmap.h>

void Init(struct Gamestate*);
void Update(uint32_t);
void Draw(Bitmap* surface);

Gamestate InitState = { Init, NULL, NULL, Update, Draw };
Game* TheGame = &(Game) {&InitState};

void Init(struct Gamestate* state)
{
	EnableDebugOutput(DEBUG_USART);
	printf("Init controller test\r\n");
	setFont(fontblack8);
}

snes_button_state_t state1;
snes_button_state_t state2;

void Update(uint32_t delta)
{
	state1 = GetControllerState1();
  state2 = GetControllerState2();
}

void OffsDrawFilledRectangle(Bitmap *bitmap,int x,int y,int w,int h, Pixel c) {
  DrawFilledRectangle(bitmap, x+5, y+10, w, h, c);
}

void Draw(Bitmap* surface)
{
  memset(surface->pixels,255,surface->bytesperrow*surface->height);

	DrawText(surface, "Controller Test v0.1", 5, 1);

  DrawRLEBitmap(surface, controller, 5, 10);

  if (state1.buttons.Up )
    OffsDrawFilledRectangle(surface,  67, 81, 10, 15, RGB(255,0,0));
  if ( state1.buttons.Down )
    OffsDrawFilledRectangle(surface,  65, 115, 10, 15, RGB(255,0,0));
  if (state1.buttons.Left )
    OffsDrawFilledRectangle(surface,  48, 100, 15, 10, RGB(255,0,0));
  if (state1.buttons.Right )
    OffsDrawFilledRectangle(surface,  85, 100, 15, 10, RGB(255,0,0));
  if (state1.buttons.Start )
    OffsDrawFilledRectangle(surface, 155, 103, 15, 15, RGB(255,0,0));
  if (state1.buttons.Select )
    OffsDrawFilledRectangle(surface, 120, 103, 15, 15, RGB(255,0,0));
  if (state1.buttons.A )
    OffsDrawFilledRectangle(surface, 268,  95, 20, 20, RGB(255,0,0));
  if (state1.buttons.B )
    OffsDrawFilledRectangle(surface, 238, 115, 20, 20, RGB(255,0,0));
  if (state1.buttons.X )
    OffsDrawFilledRectangle(surface, 240,  74, 20, 20, RGB(255,0,0));
  if (state1.buttons.Y )
    OffsDrawFilledRectangle(surface, 209,  96, 20, 20, RGB(255,0,0));
  if (state1.buttons.L )
    OffsDrawFilledRectangle(surface,  48,  32, 50,  5, RGB(255,0,0));
  if (state1.buttons.R )
    OffsDrawFilledRectangle(surface, 216,  32, 50,  5, RGB(255,0,0));

  if (state2.buttons.Up )
    OffsDrawFilledRectangle(surface,  67, 81, 10, 15, RGB(0,255,0));
  if ( state2.buttons.Down )
    OffsDrawFilledRectangle(surface,  65, 115, 10, 15, RGB(0,255,0));
  if (state2.buttons.Left )
    OffsDrawFilledRectangle(surface,  48, 100, 15, 10, RGB(0,255,0));
  if (state2.buttons.Right )
    OffsDrawFilledRectangle(surface,  85, 100, 15, 10, RGB(0,255,0));
  if (state2.buttons.Start )
    OffsDrawFilledRectangle(surface, 155, 103, 15, 15, RGB(0,255,0));
  if (state2.buttons.Select )
    OffsDrawFilledRectangle(surface, 120, 103, 15, 15, RGB(0,255,0));
  if (state2.buttons.A )
    OffsDrawFilledRectangle(surface, 268,  95, 20, 20, RGB(0,255,0));
  if (state2.buttons.B )
    OffsDrawFilledRectangle(surface, 238, 115, 20, 20, RGB(0,255,0));
  if (state2.buttons.X )
    OffsDrawFilledRectangle(surface, 240,  74, 20, 20, RGB(0,255,0));
  if (state2.buttons.Y )
    OffsDrawFilledRectangle(surface, 209,  96, 20, 20, RGB(0,255,0));
  if (state2.buttons.L )
    OffsDrawFilledRectangle(surface,  48,  32, 50,  5, RGB(0,255,0));
  if (state2.buttons.R )
    OffsDrawFilledRectangle(surface, 216,  32, 50,  5, RGB(0,255,0));

}
