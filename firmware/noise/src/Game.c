#include <game/Game.h>
#include <game/Debug.h>
#include <game/Audio.h>

#include <stdio.h>
#include <string.h>

static const int SPEED = 4000;

void Init(struct Gamestate*);
void Update(uint32_t);
void Draw(Bitmap *);

Gamestate InitState = { Init, NULL, NULL, Update, Draw };
Game* TheGame = &(Game) {&InitState};

void Init(struct Gamestate* state)
{
	InitializeAudio(Gaming_AudioFreq_11k);

	InitializeLEDs();
	SetLEDs(0x01);
}

void Update(uint32_t a) {
	uint16_t led = 0;

	for (uint16_t t = 0;; t++) {
		uint16_t sample = t*5&(t>>7)|t*3&(t*4>>10);
		OutputAudioSample(sample);

		if ((led % SPEED) == 0) {
			if (led >= SPEED * 4) led = 0;
			SetLEDs(1<<(led / SPEED));
		}
		led++;
	}
}

void Draw(Bitmap *b)
{
}
