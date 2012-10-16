#include <game/Game.h>
#include <stdio.h>

int ChangeState(Game *game, Gamestate *state) {
	if(!state->Update || !state->Draw) {
		fprintf(stderr, "PANIC: Update and/or Draw function pointer is NULL\r\n");
		return -1;
	}
	if(state->previousState != NULL) {
		fprintf(stderr, "This Gamestate is already in the Stack\r\n");
		return -1;
	}
	state->previousState = game->currentState;
	game->currentState = state;
}

void ExitState(Game* game) {
	Gamestate* state = game->currentState;
	if (state->previousState)
	{
		game->currentState = state->previousState;
		state->previousState = NULL;
	}
}