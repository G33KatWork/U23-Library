#ifndef _GAME_H_
#define _GAME_H_

#include <stdint.h>
#include <platform/SNES.h>
#include <platform/LED.h>
#include <platform/SNES.h>
#include <platform/SysTick.h>
#include <platform/VGA.h>
#include <platform/Random.h>

#include <game/Gamestate.h>

#include <Drawing.h>

typedef struct {
	Gamestate *currentState;
} Game;

extern Game* TheGame;

#endif
