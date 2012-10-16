#ifndef _GAME_H_
#define _GAME_H_

#include <stdint.h>
#include <platform/SNES.h>
#include <platform/LED.h>
#include <platform/SNES.h>
#include <platform/SysTick.h>
#include <platform/VGA.h>
#include <platform/Random.h>

#include <Drawing.h>

typedef struct Gamestate {
	bool initialized;
	void (*Init)(struct Gamestate* state);		// executed once entering the state
	void (*OnEnter)(struct Gamestate* state);	// executed on entering the state
	void (*OnLeave)(struct Gamestate* state);	// executed before leaving the state
	void (*Update)(uint32_t);					// executed every frame before Draw
	void (*Draw)(Bitmap* surface);				// executed every frame after Update
	struct Gamestate *previousState;
} Gamestate;

typedef struct {
	Gamestate *currentState;
} Game;

extern Game* TheGame;

int ChangeState(Game *game, Gamestate *state);
void ExitState(Game* game);

#endif
