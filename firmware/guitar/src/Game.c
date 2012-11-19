#include <game/Game.h>
#include <game/Debug.h>
#include <game/Audio.h>

#include <stdio.h>
#include <string.h>

#include "AudioData.h"

#define AudioFreq Gaming_AudioFreq_44k

static const int SPEED = 40;

void Init(struct Gamestate*);
void Update(uint32_t);
void Draw(Bitmap *);

Gamestate InitState = { Init, NULL, NULL, Update, Draw };
Game* TheGame = &(Game) {&InitState};

static void AudioCallback(void *context, int16_t buffer[256])
{
	static uint32_t pos = 0;

	for(int i = 0; i < 128; i++) {
		buffer[2*i+0] = buffer[2*i+1] = data[pos];
		pos = (pos + 1) % 220500; // 5 sec 44.1 kHz Audio
	}
}

void Init(struct Gamestate* state)
{
	InitializeAudio(AudioFreq);

	InitializeLEDs();
	SetLEDs(0x01);

	SetAudioVolume(0xff);
	PlayAudioWithCallback(AudioCallback, NULL);
}

void Update(uint32_t a)
{
	static uint16_t led = 0;

	if ((led++ % SPEED) == 0) {
		if (led >= SPEED * 4) led = 0;
		SetLEDs(1<<(led / SPEED));
	}
}

void Draw(Bitmap *b)
{
}
