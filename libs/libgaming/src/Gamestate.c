#include "Gamestate.h"

void changeGamestate(Game *game, Gamestate *state) {
	if(!state->Update || !state->Draw) {
		fprintf(stderr, "PANIC: Update and/or Draw function pointer is NULL\r\n");
		return -1;
	}
	game->currentState = state;
	state->Init(game);
}

void stateReturn(Game game) {
	if (game.previousState)
	{
		game.currentState = game.previousState;
	}
}