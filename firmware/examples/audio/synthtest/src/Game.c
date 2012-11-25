#include <game/Game.h>
#include <Debug.h>
#include <game/Audio.h>
#include <game/Synth.h>

#include <stdio.h>
#include <string.h>

#define AudioFreq Gaming_AudioFreq_22k

static const int SPEED = 40;

void Init(struct Gamestate*);
void Update(uint32_t);
void Draw(Bitmap *);

Gamestate InitState = { Init, NULL, NULL, Update, Draw };
Game* TheGame = &(Game) {&InitState};

SynthSong song = {
	AudioFreq,
	1,
	&(SynthChannel) {
		SynthRect,
		19*2,
		(SynthNote[]) {
			{ Note_a1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_a1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_c2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_a1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_a2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_e2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_d2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_c2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_d2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_e2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_a1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_a1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_c2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_a1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_h1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_h1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_a1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_c2, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
			{ Note_g1, 250, 0xff },
			{ Note_Pause, 62, 0x00 },
		}
	}
};

static void AudioCallback(void *context, int16_t buffer[256])
{
	for(int i = 0; i < 128; i++) {
		buffer[2*i+0] = buffer[2*i+1] = SynthGetSample(&song);
	}
}

void Init(struct Gamestate* state)
{
	InitializeAudio(AudioFreq);

	InitializeLEDs();
	SetLEDs(0x01);

	SetAudioVolume(0xa0);
	PlayAudioWithCallback(AudioCallback, NULL);
}

void Update(uint32_t a) {
	static uint16_t led = 0;

	if ((led++ % SPEED) == 0) {
		if (led >= SPEED * 4) led = 0;
		SetLEDs(1<<(led / SPEED));
	}
}

void Draw(Bitmap *b)
{
}
