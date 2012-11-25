#include <game/Game.h>
#include <Debug.h>
#include <game/Filesystem.h>
#include <game/Accelerometer.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>
#include <stm32f4xx/stm32f4xx_rng.h>
#include <math.h>

#include "../inc/Highscore.h"
#include "../inc/Sprites.h"

//#define ACCELEROMETER

static uint32_t sqrti(uint32_t n);

void Init(struct Gamestate*);
void OnEnter(struct Gamestate*);
void Update(uint32_t );
void Draw(Bitmap* surface);

Gamestate game = { Init, OnEnter, NULL, Update, Draw, };

#ifdef ACCELEROMETER
static uint32_t sqrti(uint32_t n);
static int32_t acc_x=0,acc_y=0;
#endif

static int32_t highscore = 0;
static int32_t lastMenuUpdate;
static int32_t gameOverEntryTime;
static int pause_mode = 0;
static int gameover = 0;

int pos_x = (320-42)/2;
int pos_y = (200-42)/2;
int dir = 0;

#define NumberOfStars 3
static struct Star
{
	int x,y,dy,f;
} stars[NumberOfStars];

static inline bool RectRectIntersection(int16_t x1, int16_t y1, int16_t w1, int16_t h1, int16_t x2, int16_t y2, int16_t w2, int16_t h2)
{
	return (x1 <= x2+w2 && x2 <= x1+w1 && y1 <= y2+h2 && y2 <= y1+h1);
}

void gameOver(Bitmap *currframe){
	if (gameover) {
		if (currframe)
		{
			DrawFilledRectangle(currframe,0,0,320,200,RGB(255,0,0));
		}

		if (highscore)
		{
			WriteHighscore(highscore);
		}

		DrawText(currframe, "GAME OVER", 50, 50);
		highscore = 0;

		for (int i = 0; i < NumberOfStars; ++i)
		{
			stars[i].y = 400;
		}

		pos_x = (320-42)/2;
		pos_y = (200-42)/2;
		dir = 0;
	} 
}

void OnEnter(struct Gamestate* state) {
	highscore = 0;
	pause_mode = 0;
	gameOver(NULL);
	gameover = 0;
}

void Init(struct Gamestate* state)
{
	EnableDebugOutput(DEBUG_USART);
	printf("Init\r\n");

	InitializeLEDs();

	SetLEDs(0x01);
	SetLEDs(0x07);

#ifdef ACCELEROMETER
	InitializeAccelerometer();
	printf("Init Accelerometer: %s\r\n", PingAccelerometer() > 0 ? "OKAY" : "FAILED");
	CalibrateAccelerometer();
#endif

	for(int i=0;i<NumberOfStars;i++)
	{
		stars[i].x=GetRandomInteger()%360;
		stars[i].y=(GetRandomInteger()%200);

		int z=sqrti((NumberOfStars-1-i)*NumberOfStars)*1000/NumberOfStars;
		stars[i].dy=1;//6000*1200/(z+200);

		stars[i].f=(6-(z*7)/1000)+(GetRandomInteger()%6)*7;
	}
}

void Draw(Bitmap *surface) {
	ClearBitmap(surface);

	if (pause_mode)
	{
		setFont(fontwhite16);
		DrawText(surface, "PAUSE", 50, 180);
	} else if (gameover) {
		gameOver(surface);
		if (SysTickCounter - gameOverEntryTime > 200)
		{
			gameover = 0;
		}
	} else {
		for(int i=0;i<NumberOfStars;i++)
		{
			DrawRLEBitmap(surface,sprites[stars[i].f],(stars[i].x),stars[i].y);

			stars[i].y+=stars[i].dy;
			if(stars[i].y>=(200))
			{
				stars[i].y=(0);
				stars[i].x=GetRandomInteger()%320;
				stars[i].f=(stars[i].f%7)+(GetRandomInteger()%6)*7;

			}
			const RLEBitmap sprite = *sprites[stars[i].f];
			const RLEBitmap spaceBitmap = *spacecraft[0];
			if (RectRectIntersection(stars[i].x, stars[i].y, sprite.width, sprite.height, pos_x, pos_y, spaceBitmap.width, spaceBitmap.height))
			{
				gameover = 1;
				gameOverEntryTime = SysTickCounter;
			}
		}
	}

	setFont(fontwhite16);
	char *highscoreString;
	asprintf(&highscoreString, "%d", highscore);
	DrawText(surface, highscoreString, 0, 8);
	free(highscoreString);

	DrawRLEBitmap(surface, spacecraft[dir+1], pos_x, pos_y);
}

void Update(uint32_t delta) {
	int32_t old_pos_x = pos_x;
	int32_t old_pos_y = pos_y;

	if ((SysTickCounter - lastMenuUpdate) > 40 )
	{
		if (GetControllerState1().buttons.Start) {
			lastMenuUpdate = SysTickCounter;
			pause_mode = pause_mode ? 0 : 1;
		} else if (GetControllerState1().buttons.Select) {
			ExitState();
		}
	}

	if (pause_mode)
	{
		return;
	} else {
		highscore += delta;
	}

	#ifdef ACCELEROMETER
		/*if (UserButtonState()) {
			ReadRawAccelerometerData(zero);
		}*/


		int8_t components[3];
		ReadCalibratedAccelerometerData(components);
		int32_t dx = components[1];
		int32_t dy = components[0];

		dx = (dx / 10) * 10;
		dy = (dy / 10) * 10;

		pos_x -= (dx / 10) * delta;
		pos_y -= (dy / 10) * delta;
	#else
		pos_x += GetControllerState1().buttons.Right * delta;
		pos_x -= GetControllerState1().buttons.Left * delta;
		pos_y -= GetControllerState1().buttons.Up *delta;
		pos_y += GetControllerState1().buttons.Down *delta;
	#endif

	dir = 0;

	if (old_pos_x < pos_x) {
		dir = 1;
	} else if (old_pos_x > pos_x) {
		dir = -1;
	}

	if (pos_x < 0) {
		pos_x = 0;
	}

	if (pos_x >= 320) {
		pos_x = 320;
	}

	if (pos_y >= 200) {
		pos_y = 200;
	}

	if (pos_y < 0) {
		pos_y = 0;
	}

}

static uint32_t sqrti(uint32_t n)
{
	uint32_t s,t;

	#define sqrtBit(k) \
	t = s+(1UL<<(k-1)); t <<= k+1; if (n >= t) { n -= t; s |= 1UL<<k; }

	s=0;
	if(n>=1<<30) { n-=1<<30; s=1<<15; }
	sqrtBit(14); sqrtBit(13); sqrtBit(12); sqrtBit(11); sqrtBit(10);
	sqrtBit(9); sqrtBit(8); sqrtBit(7); sqrtBit(6); sqrtBit(5);
	sqrtBit(4); sqrtBit(3); sqrtBit(2); sqrtBit(1);
	if(n>s<<1) s|=1;

	#undef sqrtBit

	return s;
}
