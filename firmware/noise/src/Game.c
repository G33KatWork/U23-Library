#include <game/Game.h>
#include <game/Debug.h>
#include <game/Audio.h>

#include <stdio.h>
#include <string.h>

void Init(struct Gamestate*);
void Update(uint32_t);
void Draw(Bitmap *);

Gamestate InitState = { Init, NULL, NULL, Update, Draw };
Game* TheGame = &(Game) {&InitState};

void Init(struct Gamestate* state)
{
	InitializeAudio();
}

void Update(uint32_t a) {
	uint16_t noise = 0;

	while (1) {
		noise = noise * 1103515245u + 12345u;
		OutputAudioSample(noise - INT16_MIN);
	}
}

void Draw(Bitmap *b)
{
}
